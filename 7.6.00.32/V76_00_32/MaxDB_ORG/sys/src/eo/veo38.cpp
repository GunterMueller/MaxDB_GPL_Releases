/*!
  @file           veo38.cpp
  @author         ThomasA,FerdinandF,MarkusSi
  @special area   COM_Interface
  @brief          COM_Interface

  @see

\if EMIT_LICENCE

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



\endif
*/
/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/
#if defined(NMP)
#define RWSTD_NO_NAMESPACE  1
#include <sys/user.h>
#endif

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "ggg01.h"
#include "ggg92.h"
#include "heo01.h"
#include "heo52.h"
#include "heo58.h" //vGetAcvPtrFromCurrentTask
#include "heo15.h"
#include "RunTime/System/RTESys_IO.h"
#include "heo38.h"
#include "liveCache/LVC_Dispatcher.hpp" // for DCOM memory
#include "liveCache/LVC_LoadLibrary.hpp" // for DCOM memory
#include "hak42.h"    // a42ResetOmsMonitor


#if defined (_WIN32)
#include "heo00.h" /* nocheck */
#include "gos00.h" /* nocheck */
/* gos00.h defines try as '__try' and catch as '__except' */
#undef try
#undef catch
#include "gos00k.h"           /* nocheck */
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "hos38.h"            /* nocheck */

#else
#include "heo00.h" /* nocheck */
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "vsp002c.h" /* nocheck */
#include "heo46.h" /* nocheck */
#include "geo60.h" /* nocheck */
#include "heo60.h" /* nocheck */
#include "gen50.h" /* nocheck */
#include "hen38.h" /* nocheck */
#endif

#include "geo007_1.h"

#include "hgg08.h"   // semaphore definition
#include "heo51.h"   // vgetpid
#include "heo55k.h"  // vbegexcl, vendexcl
#include "heo56.h"   // vsleep
#include "geo60.h"

#include "geo00d.h"
#include "hak071.h"
#include "hak34.h"
#include "hak341.h"
#include "geo38.h"
#include "heo922.h"  // vGetRteInfoTaskDetails
#include "hsp77.h"
#include "Oms/OMS_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "liveCache/LVC_Dispatcher.hpp"
#include "liveCache/LVC_ContainerManager.hpp"
#include "SQLManager/Catalog/Catalog_Types.hpp"

#include "heo02x.h"
#define  KERNEL        1        /* PTS 1115295 FF 2002-04-19 FF */
//#include "Oms/OMS_DbpError.hpp"
#include "ggg251.h"

#include "geo50_0.h"   /* PTS 1106969 */
#include "hgg01.h"     /* PTS 1110818 */

#undef _CDECL       // PTS 1112369
#include "geo573.h" // PTS 1112369
#include "Oms/OMS_CallbackInterface.hpp"
#include "Oms/OMS_VarObj.hpp"      // PTS 1121449
#include "SQLManager/SQLMan_Context.hpp"
externC void vabort(tsp00_Bool dumpCore);

/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

/* +++  PTS 1106969  x_cons information extended                        */
#define ENTER_SINK_METHOD(_idx)  this->CallbackIsActive = true;\
  if ( this->lpTaskContext != NULL ) { \
    TASK_STATE(REINTERPRET_CAST(TASK_CTRL,this->lpTaskContext)) = TSK_RUNNING_EO00;\
    REINTERPRET_CAST(TASK_CTRL,this->lpTaskContext)->TaskOMSStat.CallbackCount++;\
    REINTERPRET_CAST(TASK_CTRL,this->lpTaskContext)->TaskOMSStat.OMSCallActive = 1;\
    REINTERPRET_CAST(TASK_CTRL,this->lpTaskContext)->TaskOMSStat.MethodIdx = _idx;\
  }

#define LEAVE_SINK_METHOD(_rc)  this->CallbackIsActive = false;\
  if ( this->lpTaskContext != NULL ) {\
    TASK_STATE(REINTERPRET_CAST(TASK_CTRL,lpTaskContext)) = TSK_DCOM_OBJ_CALLED_EO00;\
    REINTERPRET_CAST(TASK_CTRL,this->lpTaskContext)->TaskOMSStat.OMSCallActive = 0;\
  };\
  return _rc
/* ---  PTS 1106969  x_cons information extended                        */

/*---------------------------------------------------------------------------*/


/*===========================================================================*
*  MACROS                                                                   *
*===========================================================================*/

/*===========================================================================*
*  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
*===========================================================================*/

namespace veo38 {
    class ContextScope 
    {
    public :
        ContextScope (tak_all_command_glob& acv)
            : m_context(*reinterpret_cast<SQLMan_Context*>(&acv))
        {
            m_context.ResetReturnCode();
            m_context.ResetErrorList();
        }
        ~ContextScope()
        {
           m_context.ResetReturnCode();
           m_context.ResetErrorList();
        }
        SQLMan_Context& GetContext()
        {
            return m_context;
        }
    private :
        SQLMan_Context& m_context;
    };
};


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
*===========================================================================*/

/*===========================================================================*
*  GLOBAL VARIABLES                                                         *
*===========================================================================*/

static tsp00_Int4                                                    DummyBadAllocCountDown = -1;


/*===========================================================================*
*  LOCAL VARIABLES                                                          *
*===========================================================================*/

/*===========================================================================*
*  LOCAL FUNCTIONS (PROTOTYPES)                                             *
*===========================================================================*/

/*===========================================================================*
*  GLOBAL FUNCTIONS (CODE)                                                  *
*===========================================================================*/

/*===========================================================================*
*                                                                            *
*===========================================================================*/

/*------------- +++++ PTS 1106505 FF +++++++++++++++ ------------------------*/

#if defined (WIN32)
#include "hos38.h"

static void eo38CTraceBackLL( tsp00_Addr hd )
{
  __try                                /* PTS 1110787 */
  {                                    /* PTS 1110787 */
    eo38_Dump_Exception ( REINTERPRET_CAST(LPEXCEPTION_POINTERS, hd) );
  }
  __except (EXCEPTION_EXECUTE_HANDLER) /* PTS 1110787 */
  {                                    /* PTS 1110787 */
  }                                    /* PTS 1110787 */
}

externC void eo38CTraceBack( tsp00_Addr hd )
{
  RTESync_LockedScope lck(LVC_Dispatcher::getDCOMSpinlock());
  eo38CTraceBackLL(hd);
}
#else
externC void eo38CTraceBack( tsp00_Addr hd )
{}
#endif

/* --- PTS 1106505 FF ---------------  */

externC int eo38_Eval_Exception ( tsp_dcom_dispatch_parms * parms)
{
  IID loc_iid;
  IUnknown                *lpIUnknown;
  IClassFactory           *lpIFactory;
  LVC_DispatcherCOMClassHash           *HashPtr;
  LVC_DispatcherHashEntryPtr           currEntry;

  parms->hresult = GEO00D_DCOM_DBPROC_CRASHED;
  if (parms->session_context != NULL )
  {
    tsp00_C4          session;
    tsp1_packet*      SQLBuffer_ptr;
    CIliveCacheSink*  pIliveCacheSink;
    ak341GetDebugContext ((tak_all_command_glob*)parms->session_context,
        *REINTERPRET_CAST(tgg91_SessionNo*, &session),
        *REINTERPRET_CAST(tsp00_Addr*, &pIliveCacheSink),
        SQLBuffer_ptr );
    if ( !pIliveCacheSink->CallbackIsActive)
      parms->hresult = -1; // stay alive
  }
  MSGD ((ERR_DCOM_HRESULT, "eo38VDcomDispatch", parms->hresult ));
  lpIUnknown = (IUnknown *) parms->idispatch;
  lpIFactory = (IClassFactory *) parms->ifactory;

  RTESync_LockedScope lck(LVC_Dispatcher::getDCOMMemory().lExclusive);
  HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
  if ( NULL != HashPtr )
  {
    /* Release session related debugger                        */
    HashPtr->ResetEntrySearch( 0, LVC_DispatcherCOMClassHash::SearchALL );
    currEntry = HashPtr->NextEntry();
    while ( NULL != currEntry)
    {
      if ((currEntry->getIUnknown() == lpIUnknown) ||
        (currEntry->getIUnknown() == lpIFactory))
      {
        SAPDB_MemCopyNoCheck (&loc_iid.Data1, currEntry->getGuid(), sizeof (loc_iid));
        if (currEntry->getIUnknown() == lpIUnknown)
        {
          MSGD ((ERR_DCOM_COMETHOD_CRASHED,
            loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
            loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
            loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
            loc_iid.Data4[6], loc_iid.Data4[7], parms->dispid));
        }
        else
        {
          MSGD ((ERR_DCOM_DBPROC_CRASHED,
            loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
            loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
            loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
            loc_iid.Data4[6], loc_iid.Data4[7], parms->dispid));
        }
        break;
      }
      currEntry = HashPtr->NextEntry();
    }
  }
  return 0;
}

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

using namespace veo38;

CIliveCacheSink::CIliveCacheSink() 
  : lpDefaultContext(NULL)
  , lpTaskContext(NULL)
  , lpCallTaskInfo(NULL)
  , lpSqlSessionContext(NULL)     /* PTS 1120794 */
  , sqlErrorFlag(false)
  , CallbackIsActive(false)

{
  for (int ix = 0; ix < OMS_SYS_REGION; ++ix)
  {
    m_inRegion[ix] = false;
  }
}

/*---------------------------------------------------------------------------*/

void CIliveCacheSink::Destruct()
{
}

/*---------------------------------------------------------------------------*/

void *CIliveCacheSink::operator new( size_t stAllocateBlock, void *addr )
{
  return addr;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::QueryInterface ( REFIID iid, PVOID *ppv)
{
  if (memcmp (&iid, &IID_IUnknown, sizeof (IID)) == 0)
    *ppv = (void *) this;
  else if (memcmp (&iid, &IID_IliveCacheSink, sizeof (IID)) == 0)
    *ppv = (void *) this;
  else
  {
    *ppv = 0;
    return E_NOINTERFACE;
  }
  this->AddRef();
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CIliveCacheSink::AddRef(void)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

CIliveCacheSink* CIliveCacheSink::GetSessionSink()
{
  tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, vGetAcvPtrFromCurrentTask());
	if (0 != pAcv)
	{
		return REINTERPRET_CAST(CIliveCacheSink*, &pAcv->a_kernel_sink);
	}
	return 0;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(ULONG) CIliveCacheSink::Release(void)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetDefaultContext (void** p, pasbool** pCancelByte, bool& optimizedStreamCommunictation, tsp00_TaskId& taskId)
{
  *p           = lpDefaultContext;
  *pCancelByte = &this->lpSessionContext->a_transinf.tri_trans.trRteCommPtr_gg00->to_cancel;
  optimizedStreamCommunictation = 0 != this->lpSessionContext->a_optimizedStreamSupport;
  taskId = this->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00; // PTS 1110315
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SetDefaultContext (void* p)
{
  lpDefaultContext = p;
  return S_OK;
}

/*----- PTS 1120794 ---------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetSqlSessionContext (
  void** p,
  tsp00_TaskId  &taskId)
{
  *p           = lpSqlSessionContext;
  taskId = this->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00; // PTS 1110315
  return S_OK;
}

/*----- PTS 1120794 ---------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SetSqlSessionContext (void* p)
{
  lpSqlSessionContext = p;
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetObj (
                                      tgg91_TransNo&                 consistentView,
                                      tgg01_ContainerId&             containerId,
                                      const tgg01_OmsVersionContext* pVersionContext,
                                      const SAPDB_UInt8&             oid,
                                      bool                           doLock,
                                      bool                           shared,
                                      tgg91_PageRef&                 objSeq,
                                      tgg91_TransNo&                 updTransId,
                                      size_t                         bodySize,
                                      unsigned char*                 pBody,
                                      size_t&                        bodyLen,
                                      tsp00_Int4&                    histLogCount,
                                      tgg00_BasisError&              e)
{
  ENTER_SINK_METHOD(GetObj_method_idx);
  // If objSeq != NIL then during deref this oid is used for accessing the right version
  // in the history chain. This is only used for iterators and therefore this variable must
  // be set to nil in all other cases.
  //objSeq.gg91SetNilRef();

  tgg00_BasisError error = 0;
  tsp00_Int4       len;
  ak341GetObj (this->lpSessionContext,
    consistentView,
    containerId,
    pVersionContext,
    *REINTERPRET_CAST(const tgg92_KernelOid*, &oid),
    doLock,
    shared,
    bodySize,
    REINTERPRET_CAST(tgg00_Rec*, pBody),
    len, 
    objSeq,
    updTransId,
    histLogCount,
    error);
  bodyLen = len;
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935

  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetObjects(
    tgg91_TransNo                 &consistentView,
    tgg01_ContainerId             *pContainerId,
    const tgg01_OmsVersionContext *pVersionContext,
    tsp00_Int4                     noOfOid,
    const SAPDB_UInt8             *pOid,
    tgg91_PageRef                 *pObjVers,
    tgg91_TransNo                 *pUpdTransId,
    size_t                        *pBodySize,
    bool                           doLock,
    bool                           shared,
    unsigned char                **ppObj,
    tsp00_Int4                    &histLogCount,
    tsp00_Int4                    &errorCount,
    tgg00_BasisError              *e)
{
  ENTER_SINK_METHOD(GetObjects_method_idx);
  tsp00_Int4 dummyLen = 0;
  tsp00_Int4 cnt      = 0;
  histLogCount        = 0;
  errorCount          = 0;
  tgg00_BasisError error = 0;
  for (int i=0; i<noOfOid; ++i){
    ak341GetObj (this->lpSessionContext,
      consistentView,
      pContainerId[i],
      pVersionContext,
      *REINTERPRET_CAST(const tgg92_KernelOid*, &(pOid[i])),
      doLock,
      shared,
      pBodySize[i],
      REINTERPRET_CAST(tgg00_Rec*, ppObj[i]),
      dummyLen,
      pObjVers[i],
      pUpdTransId[i],
      cnt,
      error);
    if (error != e_ok){
      e[i] = a071_return_code(error, sqlm_internal);   // PTS 1124935
      ++errorCount;
    }
    else {
      e[i] = 0;
    }

    histLogCount += cnt;
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::UpdateObj(
    unsigned char  * lpConsistentView,
    unsigned char  * lpContainerId,
    unsigned char  * lpVersionContext,
    unsigned char  * lpoid,
    unsigned char  * lpobjseq,
    size_t           bodysize,
    unsigned char  * lpbody,
    size_t           KeyPos,     // PTS 1122540
    size_t           KeyLen,     // PTS 1122540
    bool             contObj,    // PTS 1124278
    bool             reused,     // PTS 1127661
    short          * b_error)
{
  ENTER_SINK_METHOD(UpdateObj_method_idx);
  tgg00_BasisError error = 0;
  ak341UpdateObj (this->lpSessionContext,
    (tgg91_TransNo *)          lpConsistentView,
    (tgg01_ContainerId *)       lpContainerId,
    (tgg01_OmsVersionContext*)  lpVersionContext,
    REINTERPRET_CAST(tgg92_KernelOid *,         lpoid),
    (tgg91_PageRef *)           lpobjseq,
    bodysize,
    (tgg00_Rec *) lpbody,
    KeyPos,
    KeyLen,
    contObj,
    reused,
    &error);

  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
STDMETHODIMP CIliveCacheSink::UpdateObjMass(
  tsp00_Int4               cnt,
  tgg91_TransNo           *pConsistentView,
  tgg01_ContainerId      **ppContainerId,
  unsigned char           *pVersionContext,
  SAPDB_UInt8             *pOid,
  tgg91_PageRef           *pObjSeq,
  size_t                  *pBodySize,
  unsigned char          **ppBody,
  size_t                  *pKeyPos,      // PTS 1122540
  size_t                  *pKeyLen,      // PTS 1122540
  bool                    *pReused,      // PTS 1127661
  bool                     clustered,
  tsp00_Int4              &errCnt,
  tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(UpdateObjMass_method_idx);

  if (!clustered){
    tgg00_BasisError  DBError = 0;
    errCnt = 0;
    for (int i=0; i<cnt; ++i){
      ak341UpdateObj (this->lpSessionContext,
        pConsistentView,
        ppContainerId[i],
        (tgg01_OmsVersionContext*) pVersionContext,
        REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
        &pObjSeq[i],
        pBodySize[i],
        ppBody[i],
        pKeyPos ? pKeyPos[i] : 0,     // PTS 1122540
        pKeyLen ? pKeyLen[i] : 0,     // PTS 1122540
        false, //contObj              // PTS 1124278
        pReused[i],
        &DBError);

      if (DBError != e_ok){
        pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
        DBError     = 0;
        ++errCnt;
      }
      else {
        pDBError[i] = 0;
      }
    }
  }
  else {
    // All given objects belong to the same page
    ak341UpdateClusteredObjects (this->lpSessionContext,
      *pConsistentView,
      *ppContainerId[0],
      reinterpret_cast<tgg01_OmsVersionContext*>(pVersionContext),
      cnt,
      reinterpret_cast<tgg92_KernelOid*>(pOid),
      pObjSeq,
      pBodySize,
      ppBody,
      pKeyPos,     // PTS 1122540
      pKeyLen,     // PTS 1122540
      pReused,
      errCnt,
      pDBError);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
STDMETHODIMP CIliveCacheSink::UpdateVarObjMass(
  tsp00_Int4               cnt,
  tgg91_TransNo           *pConsistentView,
  tgg01_ContainerId      **ppContainerId,
  unsigned char           *pVersionContext,
  SAPDB_UInt8             *pOid,
  tgg91_PageRef           *pObjSeq,
  size_t                  *pObjSize,
  size_t                  *pBodySize,
  unsigned char          **ppBody,
  SAPDB_UInt8             *pNextOid,
  bool                    *pContObj,  // PTS 1124278
  bool                    *pReused,   // PTS 1127661
  tsp00_Int4              &errCnt,
  tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(UpdateVarObjMass_method_idx);

  OMS_VarObjContainer varObjContainer;
  tgg00_BasisError    DBError = 0;

  errCnt = 0;
  for (int i=0; i<cnt; ++i) {
    tsp00_Int4     size = 0;
    unsigned char *pObj = NULL;
    if (reinterpret_cast<tgg92_KernelOid*>(&pNextOid[i])->gg92IsNil()){
      size = pBodySize[i];
      pObj = ppBody[i];
    }
    else {
      if (pBodySize[i] != 0){
        SAPDB_MemCopyNoCheck(&varObjContainer.vobjInfo[0], ppBody[i], pBodySize[i]);
      }
      varObjContainer.m_vobjSize = pObjSize[i];
      varObjContainer.m_vobjNext = *(OMS_ObjectId8*)&pNextOid[i];

      size = sizeof(OMS_VarObjContainer);
      pObj = (unsigned char*) &varObjContainer;
    }

    ak341UpdateObj (this->lpSessionContext,
      pConsistentView,
      ppContainerId[i],
      (tgg01_OmsVersionContext*) pVersionContext,
      REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
      &pObjSeq[i],
      size,
      pObj,
      0,
      0,
      pContObj[i],
      pReused[i],
      &DBError);

    if (DBError != e_ok){
      pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
      DBError     = 0;
      ++errCnt;
    }
    else {
      pDBError[i] = 0;
    }
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
STDMETHODIMP CIliveCacheSink::NewObjMass(
    tsp00_Int4               cnt,
    tgg91_TransNo           *pConsistentView,
    tgg01_ContainerId      **ppContainerId,
    unsigned char           *pVersionContext,
    size_t                  *pKeyPos,
    size_t                  *pKeyColSize,
    unsigned char          **ppKeyColumn,
    bool                    *pContObj,    // PTS 1124278
    SAPDB_UInt8             *pOid,
    tgg91_PageRef           *pObjSeq,
    tgg91_TransNo           *pUpdTransId,
    tsp00_Int4              &errCnt,
    tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(NewObjMass_method_idx);
  tgg00_BasisError DBError = 0;
  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    ak341NewObj (this->lpSessionContext,
      pConsistentView,
      ppContainerId[i],
      (tgg01_OmsVersionContext*)  pVersionContext,
      pKeyPos     == NULL ? 0    : pKeyPos[i],
      pKeyColSize == NULL ? 0    : pKeyColSize[i],
      (tsp00_MoveObj*) ppKeyColumn == NULL ? NULL : ppKeyColumn[i],
      pContObj[i],
      REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
      &pObjSeq[i],
      &pUpdTransId[i],
      &DBError);
    if (DBError != e_ok){
      pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
      DBError     = 0;
      ++errCnt;
    }
    else {
      pDBError[i] = 0;
    }
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::NewObj(
    unsigned char   * lpConsistentView,
    unsigned char   * lpContainerId,
    unsigned char   * lpVersionContext,
    size_t            KeyPos,
    size_t            KeyColSize,
    unsigned char   * KeyColumn,
    bool              contObj,    // PTS 1124278
    SAPDB_UInt8     * lpoid,
    unsigned char   * lpobjseq,
    tgg91_TransNo   * pUpdTransId,
    short           * b_error)
{
  ENTER_SINK_METHOD(NewObj_method_idx);
  tgg00_BasisError error = 0;
  ak341NewObj (this->lpSessionContext,
    (tgg91_TransNo *)           lpConsistentView,
    (tgg01_ContainerId *)       lpContainerId,
    (tgg01_OmsVersionContext*)  lpVersionContext,
    KeyPos,
    KeyColSize,
    (tsp00_MoveObj*)    KeyColumn,
    contObj,
    REINTERPRET_CAST(tgg92_KernelOid *, lpoid),
    (tgg91_PageRef *)   lpobjseq,
    pUpdTransId,
    &error);
  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::CreateContainer(
  tsp00_Int4           SchemaId,
  GUID                *lpGUID,
  tsp00_KnlIdentifier *lpClassName,
  size_t               size,
  OmsContainerNo       ContainerNo,
  size_t               keyPos,         // PTS 1122540
  size_t               keyLen,         // PTS 1122540
  bool                 keyPartitioned, // PTS 1122540
  bool                 useCachedKeys,
  bool                 isVarObject,
  bool                 useInternalTrans,
  bool                 cNoWait,
  unsigned char       *lpContainerId,
  short               *b_error)
{
  ENTER_SINK_METHOD(CreateContainer_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  LVC_ContainerManager::RegistrationResult registrationResult = LVC_ContainerManager::RegisterContainer (context,
        SchemaId, ContainerNo, *REINTERPRET_CAST(Catalog_GUID*, lpGUID), keyPartitioned, useCachedKeys, isVarObject, cNoWait, size,
        keyPos, keyLen, *REINTERPRET_CAST(SQLMan_Identifier*, lpClassName), useInternalTrans,
        *REINTERPRET_CAST(tgg00_FileId*, lpContainerId));
  switch (registrationResult) // PTS 1135739, T.A.
  {
  case LVC_ContainerManager::REG_OK :
      *b_error = e_container_already_exists;
      break;
  case LVC_ContainerManager::REG_CREATED  :
      *b_error = 0;
      break;
  default :
      *b_error = context.ReturnCode();
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DeleteAllObj(
                                           tgg91_TransNo&   consistentView,
                                           unsigned char  * lpContainerId,
                                           unsigned char  * lpVersionContext,
                                           int            & lpDeleted,
                                           short          & b_error,
                                           SAPDB_UInt8    & errorOid)
{
    ENTER_SINK_METHOD(DeleteAllObj_method_idx);
    tgg00_BasisError error = 0;
    ak341DeleteAllObj (this->lpSessionContext,
        consistentView,
        (tgg01_ContainerId*) lpContainerId,
        (tgg01_OmsVersionContext*) lpVersionContext,
        lpDeleted,
        error,
        *REINTERPRET_CAST(tgg92_KernelOid*, &errorOid));
    b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
    LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DeleteObj(
                                        unsigned char  * lpConsistentView,
                                        unsigned char  * lpContainerId,
                                        unsigned char  * lpVersionContext,
                                        unsigned char  * lpoid,
                                        unsigned char  * lpobjseq,
                                        bool             reused,  // PTS 1127661
                                        bool             contObj, // PTS 1124278
                                        short          * b_error)
{
  ENTER_SINK_METHOD(DeleteObj_method_idx);
  tgg00_BasisError error = 0;
  ak341DeleteObj (this->lpSessionContext,
    (tgg91_TransNo *)            lpConsistentView,
    (tgg01_ContainerId *)        lpContainerId,
    (tgg01_OmsVersionContext*)   lpVersionContext,
    (tgg92_KernelOid *)          lpoid,
    (tgg91_PageRef *)            lpobjseq,
    reused,
    contObj,
    &error);

  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}


/*---------------------------------------------------------------------------*/
// PTS 1121449
STDMETHODIMP CIliveCacheSink::DeleteObjMass(
                                        tsp00_Int4               cnt,
                                        tgg91_TransNo           *pConsistentView,
                                        tgg01_ContainerId      **ppContainerId,
                                        unsigned char           *pVersionContext,
                                        SAPDB_UInt8             *pOid,
                                        tgg91_PageRef           *pObjSeq,
                                        bool                    *pReused, // PTS 1127661
                                        bool                    *pContObj,// PTS 1124278
                                        bool                     clustered, 
                                        tsp00_Int4              &errCnt,
                                        tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(DeleteObjMass_method_idx);

  if (!clustered){
    tgg00_BasisError DBError = 0;
    errCnt = 0;
    for (int i=0; i<cnt; ++i){
      ak341DeleteObj (this->lpSessionContext,
        pConsistentView,
        ppContainerId[i],
        (tgg01_OmsVersionContext*) pVersionContext,
        REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
        &pObjSeq[i],
        pReused[i],
        pContObj[i],
        &DBError);

      if (DBError != e_ok){
        pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
        DBError     = 0;
        ++errCnt;
      }
      else {
        pDBError[i] = 0;
      }
    }
  }
  else {
    // All given objects belong to the same page
    ak341DeleteClusteredObjects (this->lpSessionContext,
      *pConsistentView,
      *ppContainerId[0],
      reinterpret_cast<tgg01_OmsVersionContext*>(pVersionContext),
      cnt,
      reinterpret_cast<tgg92_KernelOid*>(pOid),
      pObjSeq,
      pReused,
      errCnt,
      pDBError);
  }

  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::CloseVersion(
                      short  *b_error)  
{
  ENTER_SINK_METHOD(CloseVersion_method_idx);
  tgg00_BasisError error = 0;
  ak341CloseVersion (this->lpSessionContext,
    &error);
  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::CreateVersion(
                      unsigned char  *lpConsistentView,
                      unsigned char  *lpVersionContext,
                      short          *b_error)
{
  ENTER_SINK_METHOD(CreateOpenVersion_method_idx);
  tgg00_BasisError error = 0;
  ak341CreateVersion (*this->lpSessionContext,
    *((tgg01_OmsVersionContext*) lpVersionContext),
    error);
  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// Interface changed: PTS 1119480
STDMETHODIMP CIliveCacheSink::CreateKeyRangeIterator(
           tgg91_TransNo            &consistentView,
           tgg00_FileId             &objFile,
           tgg01_OmsVersionContext  *lpVersionContext,
           void                    **pKeyRangeIterator,
           tsp00_Int4                objBodySize,
           size_t                    keyLen,
           const void               *pStartKey,
           const void               *pLowerKey,
           const void               *pUpperKey,
           tsp00_Int4               &noOfOid,
           SAPDB_UInt8              *pResultOId,
           tgg91_PageRef            *pObjVers,
           unsigned char            *pBody,
           tsp00_Int4               &LogReadAccesses,
           tgg00_BasisError         &e,
           OmsIterDirection          direction)
{
  LogReadAccesses = 0;
  tgg00_BasisError error = 0;
  ENTER_SINK_METHOD(CreateKeyIterator_method_idx);
  ak341CreateKeyRangeIterator (*this->lpSessionContext,
    consistentView,
    lpVersionContext,
    objFile,
    keyLen,
    pStartKey,
    pLowerKey,
    pUpperKey,
    pKeyRangeIterator,
    direction == OMS_ASCENDING ? true : false,
    error);

  if (error == 0){
    if (direction == OMS_ASCENDING){
      ak341NextObjFromKeyIterator (*this->lpSessionContext,
        consistentView,   // PTS 1127520
        objFile,
        lpVersionContext,
        *pKeyRangeIterator,
        true,              // bInclusive
        objBodySize,
        keyLen,
        CONST_CAST(void*,pStartKey),
        noOfOid,
        REINTERPRET_CAST(tgg92_KernelOid*, pResultOId),
        pObjVers,
        REINTERPRET_CAST(tsp00_MoveObj*, pBody),
        LogReadAccesses,
        error);
    }
    else {
      ak341PrevObjFromKeyIterator (*this->lpSessionContext,
        consistentView,   // PTS 1127520
        objFile,
        lpVersionContext,
        *pKeyRangeIterator,
        true,              // bInclusive
        objBodySize,
        keyLen,
        CONST_CAST(void*,pStartKey),
        noOfOid,
        REINTERPRET_CAST(tgg92_KernelOid*, pResultOId),
        pObjVers,
        REINTERPRET_CAST(tsp00_MoveObj*, pBody),
        LogReadAccesses,
        error);
    }
  }
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

// New function since: PTS 1119480
STDMETHODIMP CIliveCacheSink::StartUnloadedVersionIter(
           tgg91_TransNo           &consistentView,
           tgg01_OmsVersionContext *lpVersionContext,
           tgg00_FileId            &objFile,
           size_t                   keyLen,
           void                    *pStartKey,
           const void              *pLowerKey,
           const void              *pUpperKey,
           size_t                   objBodySize,
           void                    *pObjBody,
           SAPDB_UInt8             &ResultOId,
           short                   &e,
           OmsIterDirection        direction)
{
  ENTER_SINK_METHOD(StartUnloadedVersionIter_method_idx);
  tgg00_BasisError error = 0;
  ak341VersionGetObjViaKey (*this->lpSessionContext,
      lpVersionContext,
      keyLen,
      REINTERPRET_CAST(tsp00_MoveObj*, pStartKey),
      objFile.fileDirFileId_gg00(),
      objBodySize,
      REINTERPRET_CAST(tsp00_MoveObj*, pObjBody),
      *REINTERPRET_CAST(tgg92_KernelOid *, &ResultOId),
      error);

  if (error == e_object_not_found || error == e_hash_key_not_found /* PTS 1124369 */) {
    if (direction == OMS_ASCENDING){
      ak341VersionNextKey (*this->lpSessionContext,
          lpVersionContext,
          objFile,
          pStartKey,
          keyLen,
          false,
          CONST_CAST(void*, pUpperKey),
          *REINTERPRET_CAST(tgg92_KernelOid *, &ResultOId),
          error);
    }
    else {
      ak341VersionPrevKey (*this->lpSessionContext,
          lpVersionContext,
          objFile,
          pStartKey,
          keyLen,
          false,
          CONST_CAST(void*, pLowerKey),
          *REINTERPRET_CAST(tgg92_KernelOid *, &ResultOId),
          error);
    }
  }
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DestroyKeyRangeIterator
(
 void* pKeyRangeIterator
 )
{
  ENTER_SINK_METHOD(DestroyKeyIterator_method_idx);
  ak341DestroyKeyRangeIterator(*this->lpSessionContext, pKeyRangeIterator);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::NewConsistentView(
					  bool            isOmsVersion, /* PTS 1130354 */
                      unsigned char  *lpConsistentView,
                      short          *b_error)
{
  ENTER_SINK_METHOD(NewConsistentView_method_idx);
  tgg00_BasisError error = 0;
  ak341NewConsistentView (this->lpSessionContext,
                                isOmsVersion,
    (tgg91_TransNo *)           lpConsistentView,
    &error);
  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::EndConsistentView(
                      unsigned char  *lpConsistentView,
                      short          *b_error)
{
  ENTER_SINK_METHOD(EndConsistentView_method_idx);
  tgg00_BasisError error = 0;
  ak341EndConsistentView (this->lpSessionContext,
    (tgg91_TransNo *)           lpConsistentView,
    &error);
  *b_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::EnterCriticalSection(short regionId)
{
  ENTER_SINK_METHOD(EnterCriticalSection_method_idx);
  ak341EnterCriticalSection (this->lpSessionContext, regionId);
  if (regionId < OMS_SYS_REGION)
  {
      m_inRegion[regionId] = true;
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::LeaveCriticalSection(short regionId)
{
  ENTER_SINK_METHOD(LeaveCriticalSection_method_idx);
  ak341LeaveCriticalSection (this->lpSessionContext, regionId);
  if (regionId < OMS_SYS_REGION)
  {
      m_inRegion[regionId] = false;
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

/* PTS 1115134 */
STDMETHODIMP CIliveCacheSink::ResetVersion(
                                           unsigned char *lpVersionContext,
                                           short  &e)
{
  ENTER_SINK_METHOD(ResetVersion_method_idx);
  tgg00_BasisError error = 0;
  ak341ResetVersion (*this->lpSessionContext,
    *REINTERPRET_CAST(tgg01_OmsVersionContext*, lpVersionContext),
    error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetObjWithKey (
                                             unsigned char  * lpConsistentView,
                                             unsigned char  * lptree,
                                             unsigned char  * lpVersionContext,
                                             size_t           keylen,
                                             unsigned char  * lpkey,
                                             bool             doLock,
                                             bool             shared,
                                             size_t           bodysize,
                                             unsigned char  * lpbody,
                                             SAPDB_UInt8    * lpoid,
                                             unsigned char  * lpobjseq,
                                             tgg91_TransNo  * pUpdTransId,
                                             int            * hist_log_count, /* +++ Change Request 1000139 +++ */
                                             short          * lpb_error)
{
  ENTER_SINK_METHOD(GetObjWithKey_method_idx);
  tgg00_BasisError error = 0;
  ak341GetObjWithKey (this->lpSessionContext,
    (tgg91_TransNo *)          lpConsistentView,
    (tgg01_ContainerId *)      lptree,
    (tgg01_OmsVersionContext*) lpVersionContext,
    doLock,
    shared,
    bodysize,
    lpbody,
    keylen,
    lpkey,
    (tgg91_PageRef *)   lpobjseq,
    pUpdTransId,
    REINTERPRET_CAST(tgg92_KernelOid *, lpoid),
    hist_log_count,
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// PTS 1122194
STDMETHODIMP CIliveCacheSink::GetObjectsWithKey(
    tgg91_TransNo            *pConsistentView,
    tgg01_ContainerId        *pContainerId,
    tgg01_OmsVersionContext  *pVersionContext,
    size_t                   *pKeyLen,
    tsp00_Int4                noOfObj,
    unsigned char           **ppKey,
    bool                      doLock,
    bool                      shared,
    size_t                   *pBodySize,
    unsigned char           **ppBody,
    SAPDB_UInt8              *pOid,
    tgg91_PageRef            *pObjSeq,
    tgg91_TransNo            *pUpdTransId,
    tsp00_Int4               &histLogCnt,
    tsp00_Int4               &errorCnt,
    tgg00_BasisError         *pDBError)
{
  ENTER_SINK_METHOD(GetObjectsWithKey_method_idx);
  tgg00_BasisError DBError = 0;
  tsp00_Int4 cnt = 0;
  histLogCnt     = 0;
  errorCnt       = 0;
  for (int i=0; i<noOfObj; ++i){
    ak341GetObjWithKey (this->lpSessionContext,
      pConsistentView,
      &pContainerId[i],
      pVersionContext,
      doLock,
      shared,
      pBodySize[i],
      ppBody[i],
      pKeyLen[i],
      ppKey[i],
      &pObjSeq[i],
      &pUpdTransId[i],
      REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
      &cnt,
      &DBError);
    if (DBError != e_ok){
      pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
      DBError     = 0;
      ++errorCnt;
    }
    else {
      pDBError[i] = 0;
    }
    histLogCnt += cnt;
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SetError(short error,
                                       tsp00_Int4 buflen,
                                       const unsigned char  * lpv,
                                       tsp00_CodeType codeType)
{
  ENTER_SINK_METHOD(SetError_method_idx);
  a34SetError (*this->lpSessionContext,
    error,
    *REINTERPRET_CAST(tsp00_MoveObj*, CONST_CAST(unsigned char*, lpv)),
    buflen,
    codeType);

  this->CallbackIsActive = false;
  sqlErrorFlag = true;    /* PTS 1110814 */

  if ( g01is_livecache() )  /* 1110814 FF */
    throw RTE_DBProcError(error);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SubtransStart(
                                            short  * lpb_error)
{
  ENTER_SINK_METHOD(SubtransStart_method_idx);
  tgg00_BasisError error = 0;
  a34SubtransStart (*this->lpSessionContext,
    error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SubtransCommit(
                                             short  * lpb_error)
{
  ENTER_SINK_METHOD(SubtransCommit_method_idx);
  tgg00_BasisError error = 0;
  a34SubtransCommit (*this->lpSessionContext,
    error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SubtransRollback(
                                               short  * lpb_error)
{
  ENTER_SINK_METHOD(SubtransRollback_method_idx);
  tgg00_BasisError error = 0;
  a34SubtransRollback (*this->lpSessionContext,
    error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::Vtrace(
                                     tsp00_Int4 buflen,
                                     unsigned char  * lpv)
{
  ENTER_SINK_METHOD(Vtrace_method_idx);
  a34VTrace (*this->lpSessionContext,
    *REINTERPRET_CAST(tsp00_MoveObj*, lpv),
    buflen);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::LockObj(
                                      unsigned char  * lpConsistentView,
                                      unsigned char  * lpContainerId,
                                      SAPDB_UInt8    * lpoid,
                                      unsigned char  * lpobjseq,
                                      short          * lpb_error)
{
  ENTER_SINK_METHOD(LockObj_method_idx);
  tgg00_BasisError error = 0;
  ak341LockObj (this->lpSessionContext,
    (tgg91_TransNo *)     lpConsistentView,
    (tgg01_ContainerId *) lpContainerId,
    REINTERPRET_CAST(tgg92_KernelOid *,   lpoid),
    (tgg91_PageRef *)     lpobjseq,
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
STDMETHODIMP CIliveCacheSink::LockObjMass(
                                        tsp00_Int4              &cnt,
                                        tgg91_TransNo           *pConsistentView,
                                        tgg01_ContainerId      **ppContainerId,
                                        SAPDB_UInt8             *pOid,
                                        tgg91_PageRef           *pObjSeq,
                                        tsp00_Int8              &timeout,
                                        tsp00_Int4              &errCnt,
                                        tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(LockObjMass_method_idx);

  tgg00_BasisError DBError = 0;

  tsp00_Int2 oldTimeout    = 0;
  tsp00_Int4 time_sec      = 0;
  tsp00_Int4 time_usec     = 0;
  tsp00_Int8 currTime      = 0;
  tsp00_Int8 endTime       = 0;
  tsp00_Int8 remainingTime = 0;

  if (timeout > 0) {
    // Compute current and maximal end time
    vclock(&time_sec, &time_usec);
    currTime      = (tsp00_Int8)time_sec * 1000000 + time_usec;
    endTime       = currTime + timeout;
    remainingTime = timeout;

    // Set remaining time as timeout value for next operation and remember old value.
    oldTimeout = this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00;
    this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00 = (tsp00_Int2) (remainingTime/1000000);
  }

  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    // Lock next object in the array
    ak341LockObj (this->lpSessionContext,
      pConsistentView,
      ppContainerId[i],
      REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
      &pObjSeq[i],
      &DBError);
    if (DBError != e_ok){
      pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
      DBError     = 0;
      ++errCnt;
    }
    else {
      pDBError[i] = 0;
    }

    // Compute new remaining time and if timeout is reached, then leave loop
    if (timeout > 0){
      vclock(&time_sec, &time_usec);
      currTime      = (tsp00_Int8)time_sec * 1000000 + time_usec;
      remainingTime = endTime - currTime;
      if (remainingTime > 0 && pDBError[i] != e_request_timeout){
        // Set remaining time as timeout value for next operation.
        this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00 = (tsp00_Int2) (remainingTime/1000000);
      }
      else {
        // timeout reached; timeout is propagated by a negative output-parameter 'timeout'
        // or by pDBError[i] == e_request_timeout
        remainingTime = -1;
        cnt = i+1;   // +1  PTS 1123704
        break;
      }
    }
  }

  if (timeout > 0) {
    // Reset original timeout value
    this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00 = oldTimeout;

    // Export the remaing time
    timeout = remainingTime;
  }

  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::LockUpdObj(unsigned char  * lpConsistentView,
                                      unsigned char  * lpContainerId,
                                      SAPDB_UInt8    * lpoid,
                                      unsigned char  * lpobjseq,
                                      short          * lpb_error)
{
  ENTER_SINK_METHOD(LockUpdObj_method_idx);
  tgg00_BasisError error = 0;
  ak341LockUpdObj (this->lpSessionContext,
    (tgg91_TransNo *)     lpConsistentView,
    (tgg01_ContainerId *) lpContainerId,
    REINTERPRET_CAST(tgg92_KernelOid *,   lpoid),
    (tgg91_PageRef *)     lpobjseq,
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1121449
STDMETHODIMP CIliveCacheSink::LockUpdObjMass(
                                        tsp00_Int4               cnt,
                                        tgg91_TransNo           *pConsistentView,
                                        tgg01_ContainerId      **ppContainerId,
                                        SAPDB_UInt8             *pOid,
                                        tgg91_PageRef           *pObjSeq,
                                        bool                     clustered,
                                        tsp00_Int4              &errCnt,
                                        tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(LockUpdObjMass_method_idx);

  if (!clustered){
    tgg00_BasisError DBError = 0;
    errCnt = 0;
    for (int i=0; i<cnt; ++i){
      ak341LockUpdObj (this->lpSessionContext,
        pConsistentView,
        ppContainerId[i],
        REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
        &pObjSeq[i],
        &DBError);
      if (DBError != e_ok){
        pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
        DBError     = 0;
        ++errCnt;
      }
      else {
        pDBError[i] = 0;
      }
    }
  }
  else {
    // All given objects belong to the same page
    ak341LockUpdClusteredObjects (this->lpSessionContext,
      *pConsistentView,
      *ppContainerId[0],
      cnt,
      reinterpret_cast<tgg92_KernelOid*>(pOid),
      pObjSeq,
      errCnt,
      pDBError);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1130435
STDMETHODIMP CIliveCacheSink::LockSharedObj(
                                      unsigned char  * lpConsistentView,
                                      unsigned char  * lpContainerId,
                                      SAPDB_UInt8    * lpoid,
                                      unsigned char  * lpobjseq,
                                      short          * lpb_error)
{
  ENTER_SINK_METHOD(LockSharedObj_method_idx);
  tgg00_BasisError error = 0;
  ak341LockSharedObj (this->lpSessionContext,
    (tgg91_TransNo *)     lpConsistentView,
    (tgg01_ContainerId *) lpContainerId,
    REINTERPRET_CAST(tgg92_KernelOid *,   lpoid),
    (tgg91_PageRef *)     lpobjseq,
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1130435
STDMETHODIMP CIliveCacheSink::LockSharedObjMass(
                                        tsp00_Int4              &cnt,
                                        tgg91_TransNo           *pConsistentView,
                                        tgg01_ContainerId      **ppContainerId,
                                        SAPDB_UInt8             *pOid,
                                        tgg91_PageRef           *pObjSeq,
                                        tsp00_Int8              &timeout,
                                        tsp00_Int4              &errCnt,
                                        tgg00_BasisError        *pDBError)
{
  ENTER_SINK_METHOD(LockSharedObjMass_method_idx);

  tgg00_BasisError DBError = 0;

  tsp00_Int2 oldTimeout    = 0;
  tsp00_Int4 time_sec      = 0;
  tsp00_Int4 time_usec     = 0;
  tsp00_Int8 currTime      = 0;
  tsp00_Int8 endTime       = 0;
  tsp00_Int8 remainingTime = 0;

  if (timeout > 0) {
    // Compute current and maximal end time
    vclock(&time_sec, &time_usec);
    currTime      = (tsp00_Int8)time_sec * 1000000 + time_usec;
    endTime       = currTime + timeout;
    remainingTime = timeout;

    // Set remaining time as timeout value for next operation and remember old value.
    oldTimeout = this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00;
    this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00 = (tsp00_Int2) (remainingTime/1000000);
  }

  errCnt = 0;
  for (int i=0; i<cnt; ++i){
    // Lock next object in the array
    ak341LockSharedObj (this->lpSessionContext,
      pConsistentView,
      ppContainerId[i],
      REINTERPRET_CAST(tgg92_KernelOid *, &pOid[i]),
      &pObjSeq[i],
      &DBError);
    if (DBError != e_ok){
      pDBError[i] = a071_return_code(DBError, sqlm_internal);
      DBError     = 0;
      ++errCnt;
    }
    else {
      pDBError[i] = 0;
    }

    // Compute new remaining time and if timeout is reached, then leave loop
    if (timeout > 0){
      vclock(&time_sec, &time_usec);
      currTime      = (tsp00_Int8)time_sec * 1000000 + time_usec;
      remainingTime = endTime - currTime;
      if (remainingTime > 0 && pDBError[i] != e_request_timeout){
        // Set remaining time as timeout value for next operation.
        this->lpSessionContext->a_transinf.tri_trans.trObjReqTimeout_gg00 = (tsp00_Int2) (remainingTime/1000000);
      }
      else {
        // timeout reached; timeout is propagated by a negative output-parameter 'timeout'
        // or by pDBError[i] == e_request_timeout
        remainingTime = -1;
        cnt = i+1;   // +1  PTS 1123704
        break;
      }
    }
  }

  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::HResultError(HRESULT hresult_error)
{
  ENTER_SINK_METHOD(HResultError_method_idx);
  a34HResultError (*this->lpSessionContext,
    hresult_error);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::AllocPacket(unsigned char  ** lpv,
                                          tsp00_Int4 *sz)
{
  ENTER_SINK_METHOD(AllocPacket_method_idx);
  a34AllocPacket (*this->lpSessionContext,
    *REINTERPRET_CAST(tsp1_packet_ptr *, lpv),
    *sz);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SQLRequest(tsp00_Int4 buflen,
                                         unsigned char  * lpv)
{
  ENTER_SINK_METHOD(SQLRequest_method_idx);
  a34Request (*this->lpSessionContext,
    (tsp1_packet_ptr) lpv);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// PTS 1120478
STDMETHODIMP CIliveCacheSink::NextOids(tgg91_TransNo           &ConsistentView,
                                       tgg00_FileId            &FileId,
                                       tgg01_OmsVersionContext *pVersionContext,
                                       bool                     bOnlyHdrVarObjects,
                                       SAPDB_UInt8              CurrOid,
                                       tsp00_Int4              &noOfOid,
                                       SAPDB_UInt8             *pOid,
                                       tgg91_PageRef           *pObjVers,
                                       tsp00_Int4              &LogReadAccesses,
                                       tgg00_BasisError        *pb_error)
{
  ENTER_SINK_METHOD(NextOids_method_idx);
  ak341NextOids (*this->lpSessionContext,
    ConsistentView,
    FileId,
    pVersionContext,
    bOnlyHdrVarObjects,
    *REINTERPRET_CAST(tgg92_KernelOid*, &CurrOid),
    noOfOid,
    REINTERPRET_CAST(tgg92_KernelOid*, pOid),
    pObjVers,
    LogReadAccesses,
    *pb_error);
  *pb_error = (*pb_error == 0 ? 0 : a071_return_code(*pb_error, sqlm_internal));
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// PTS 1119480
STDMETHODIMP CIliveCacheSink::VersionPrevKey(
                            tgg01_OmsVersionContext* VersionContext,
                            const tgg01_ContainerId& ObjFile,
                            void*                    pKey,
                            size_t                   KeyLen,
                            bool                     Inclusive,
                            const void*              pLowerKey,
                            SAPDB_UInt8&             Oid,
                            tgg00_BasisError&        e)
{
  ENTER_SINK_METHOD(PrevObj_method_idx);
  tgg00_BasisError error = 0;
  ak341VersionPrevKey (*this->lpSessionContext,
      VersionContext,
      ObjFile,
      pKey,
      KeyLen,
      Inclusive,
      CONST_CAST(void*, pLowerKey),
      *REINTERPRET_CAST(tgg92_KernelOid *, &Oid),
      error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// Interface changed: PTS 1119480
STDMETHODIMP CIliveCacheSink::VersionNextKey(
                            tgg01_OmsVersionContext* VersionContext,
                            const tgg01_ContainerId& ObjFile,
                            void*                    pKey,
                            size_t                   KeyLen,
                            bool                     Inclusive,
                            const void*              pUpperKey,
                            SAPDB_UInt8&              Oid,
                            tgg00_BasisError&        e)
{
  ENTER_SINK_METHOD(NextObj_method_idx);
  tgg00_BasisError error = 0;
  ak341VersionNextKey (*this->lpSessionContext,
      VersionContext,
      ObjFile,
      pKey,
      KeyLen,
      Inclusive,
      CONST_CAST(void*, pUpperKey),
      *REINTERPRET_CAST(tgg92_KernelOid *, &Oid),
      error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// PTS 1119480
STDMETHODIMP CIliveCacheSink::PrevObjFromKeyIterator (
                            const tgg91_TransNo     &ConsistentView,  // PTS 1127520
                            tgg01_ContainerId       &ObjFile,
                            tgg01_OmsVersionContext *VersionContext,
                            void                    *pKeyIterator,
                            tsp00_Int4               objBodySize,
                            size_t                   KeyLen,
                            void                    *pRestartKey,
                            tsp00_Int4              &noOfOid,
                            SAPDB_UInt8             *pResultOId,
                            tgg91_PageRef           *pObjVers,
                            unsigned char           *pBody,
                            tsp00_Int4              &LogReadAccesses,
                            tgg00_BasisError        &e)
{
  ENTER_SINK_METHOD(PrevObjFromKeyIter_method_idx);

  tgg00_BasisError error = 0;
  LogReadAccesses = 0;

  ak341PrevObjFromKeyIterator (*this->lpSessionContext,
    ConsistentView,     // PTS 1127520
    ObjFile,
    VersionContext,
    pKeyIterator,
    false,             // bInclusive
    objBodySize, 
    KeyLen,
    pRestartKey,
    noOfOid,
    REINTERPRET_CAST(tgg92_KernelOid*, pResultOId),
    pObjVers,
    REINTERPRET_CAST(tsp00_MoveObj*, pBody),
    LogReadAccesses,
    error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

// Interface changed: PTS 1119480
STDMETHODIMP CIliveCacheSink::NextObjFromKeyIterator (
                            const tgg91_TransNo     &ConsistentView,  // PTS 1127520
                            tgg01_ContainerId       &ObjFile,
                            tgg01_OmsVersionContext *VersionContext,
                            void                    *pKeyIterator,
                            tsp00_Int4               objBodySize,
                            size_t                   KeyLen,
                            void                    *pRestartKey,
                            tsp00_Int4              &noOfOid,
                            SAPDB_UInt8             *pResultOId,
                            tgg91_PageRef           *pObjVers,
                            unsigned char           *pBody,
                            tsp00_Int4              &LogReadAccesses,
                            tgg00_BasisError        &e)
{
  ENTER_SINK_METHOD(NextObjFromKeyIter_method_idx);
  tgg00_BasisError error = 0;
	LogReadAccesses = 0;

  ak341NextObjFromKeyIterator (*this->lpSessionContext,
    ConsistentView,   // // PTS 1127520
    ObjFile,
    VersionContext,
    pKeyIterator,
    false,             // bInclusive
    objBodySize,
    KeyLen,
    pRestartKey,
    noOfOid,
    REINTERPRET_CAST(tgg92_KernelOid*, pResultOId),
    pObjVers,
    REINTERPRET_CAST(tsp00_MoveObj*, pBody),
    LogReadAccesses,
    error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ABAPRead(
    tgg01_StreamDescriptor &descriptor,
    short                  &e)
{
  ENTER_SINK_METHOD(ABAPRead_method_idx);
  tgg00_BasisError error = 0;
  a34AbapRead (*this->lpSessionContext,
      descriptor,
      error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ABAPWrite(
                                        tgg01_StreamDescriptor &descriptor,
                                        short                  &e)
{
  ENTER_SINK_METHOD(ABAPWrite_method_idx);
  tgg00_BasisError error = 0;
  a34AbapWrite (*this->lpSessionContext,
    descriptor,
    error);
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::MultiPurpose (unsigned char MessType,
                                            unsigned char MessType2,
                                            tsp00_Int4           *lpbuflen,
                                            unsigned char  *lpv,
                                            short          *lpb_error)
{
  ENTER_SINK_METHOD(MultiPurpose_method_idx);
  tgg00_MessType  l_MessType;                            /* PTS 1106619 FF */
  tgg00_MessType2 l_MessType2;                           /* PTS 1106619 FF */
  l_MessType.becomes( (tgg00_MessType_Enum)MessType );   /* PTS 1106619 FF */
  l_MessType2.becomes( (tgg00_MessType2_Enum)MessType2 );/* PTS 1106619 FF */

  ak341MultiPurpose (this->lpSessionContext,
    /*  *REINTERPRET_CAST(tgg00_MessType*,  (&MessType)),  PTS 1106619 FF */
    /*  *REINTERPRET_CAST(tgg00_MessType2*, (&MessType2)), PTS 1106619 FF */
    l_MessType,           /* PTS 1106619 FF */
    l_MessType2,          /* PTS 1106619 FF */
    REINTERPRET_CAST(int*, (lpbuflen)),
    (tsp00_MoveObj *) lpv,
    lpb_error);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::UnlockObj(unsigned char  * lpContainerId,
                                        SAPDB_UInt8    * lpoid,
                                        short          * lpb_error)
{
  ENTER_SINK_METHOD(UnlockObj_method_idx);
  tgg00_BasisError error = 0;
  ak341UnlockObj (this->lpSessionContext,
    (tgg01_ContainerId *) lpContainerId,
    REINTERPRET_CAST(tgg92_KernelOid *, lpoid),
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::UnlockSharedObj(unsigned char  * lpContainerId,
                                              SAPDB_UInt8    * lpoid,
                                              short          * lpb_error)
{
  ENTER_SINK_METHOD(UnlockSharedObj_method_idx);
  tgg00_BasisError error = 0;
  ak341UnlockSharedObj (this->lpSessionContext,
    (tgg01_ContainerId *) lpContainerId,
    REINTERPRET_CAST(tgg92_KernelOid *, lpoid),
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::IsLocked(unsigned char  * lpContainerId,
                                       SAPDB_UInt8    * lpoid,
                                       bool             shared,
                                       short          * lpb_error)
{
  ENTER_SINK_METHOD(IsLocked_method_idx);
  tgg00_BasisError error = 0;
  ak341IsLocked (this->lpSessionContext,
    (tgg01_ContainerId *) lpContainerId,
    REINTERPRET_CAST(tgg92_KernelOid *, lpoid),
    shared,
    &error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ConsoleMsg(
                                         tsp00_Int4 buflen,
                                         unsigned char  * lpv)
{
  ENTER_SINK_METHOD(ConsoleMsg_method_idx);
  unsigned char dmp_buf[132];
  if ( buflen >= sizeof(dmp_buf)) buflen = sizeof(dmp_buf) - 1;
  SAPDB_MemCopyNoCheck(dmp_buf,lpv,buflen);
  dmp_buf[buflen] = '\0';
  MSGD ((ERR_DCOM_PROXY_MSG,dmp_buf));
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DropFile(char  *lpFileName,
                                       short &error)
{
  ENTER_SINK_METHOD(DropFile_method_idx);
  tsp00_VfReturn_Param Error;
  eo15RemoveMessageFile(lpFileName, Error);
  if (vf_ok == Error) {
    error = e_ok;
  }
  else {
    char buf[132];
    sprintf(&buf[0],"DropFile: %.100s  err: %d ", lpFileName, Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ExistsFile(char  *lpFileName,
                                       bool     &doesExist,
                                       bool     &isDirectory,
                                       short    &error)
{
  ENTER_SINK_METHOD(ExistsFile_method_idx);
  tsp00_VfReturn_Param Error;
  eo15CheckExist(lpFileName, doesExist, isDirectory, Error);
  if (vf_ok == Error) {
    error = e_ok;
  }
  else {
    char buf[132];
    sprintf(&buf[0],"ExistsFile: %.100s  err: %d ", lpFileName, Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetWorkingDirectory(tsp00_Pathc& path,
                                       short  &error)
{
  ENTER_SINK_METHOD(GetWorkingDirectory_method_idx);
  tsp00_VfReturn_Param Error;
  RTESys_IOGetWorkingDirectory(path, Error);
  if (vf_ok == Error) {
    error = e_ok;
  }
  else {
    char buf[132];
    sprintf(&buf[0],"GetWorkingDirectory  err: %d ", Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetDbrootSapPath(tsp00_Pathc& path,
                                       short  &error)
{
  ENTER_SINK_METHOD(GetDbrootSapPath_method_idx);
  tsp01_RteError rteError;
  if (sqlGetDbrootSapPath (path, TERM_WITH_DELIMITER_EO01, &rteError)) {
    error = e_ok;
  }
  else {
    char buf[256];
    SAPDB_MemCopyNoCheck (&path[0], &rteError.RteErrText[0], sizeof(rteError.RteErrText));
    path[sizeof(rteError.RteErrText)] = 0;
    sprintf(&buf[0],"GetDbrootSapPath  rteError: %d %.80s", rteError.RteErrCode, rteError.RteErrText );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    sprintf(&buf[0],"GetDbrootSapPath  osError: %d %.80s", rteError.OsErrCode, rteError.OsErrText );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::OpenFile(char  *lpFileName,
                                       tsp00_Uint4 FileSize,
                                       boolean     doNotInit,
                                       tsp00_Int4  *FileHandle,
                                       short       *lpb_error)
{
  ENTER_SINK_METHOD(OpenFile_method_idx);
  tsp00_VfReturn_Param Error;
  *FileHandle = eo15OpenMessageFile(lpFileName, (teo15_Position)FileSize, (doNotInit!=0), Error);
  if (vf_ok == Error) {
    *lpb_error = e_ok;
  }
  else {
    char buf[256];
    sprintf(&buf[0],"OpenFile: %.200s filesize %d [%sforce init] err: %d ", lpFileName, FileSize, (doNotInit ? "no " : ""), Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    *lpb_error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::WriteFile(tsp00_Int4 FileHandle,
                                        char   *msg,
                                        short  *lpb_error)
{
  ENTER_SINK_METHOD(WriteFile_method_idx);
  tsp00_VfReturn_Param Error;
  eo15WriteMessageFile(FileHandle, msg, Error);
  if (vf_ok == Error) {
    *lpb_error = e_ok;
  }
  else {
    char buf[132];
    sprintf(&buf[0],"WriteFile hd: %d  err: %d ", FileHandle, Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    *lpb_error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::CloseFile(tsp00_Int4 FileHandle,
                                        short  *lpb_error)
{
  ENTER_SINK_METHOD(CloseFile_method_idx);
  tsp00_VfReturn_Param Error;
  eo15CloseMessageFile(FileHandle, Error);
  if (vf_ok == Error) {
    *lpb_error = e_ok;
  }
  else {
    char buf[132];
    sprintf(&buf[0],"CloseFile: %d  err: %d ", FileHandle, Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    *lpb_error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SyncFile(char       *lpFileName,
                                       tsp00_Int4 FileHandle,
                                       short      *lpb_error)
{
  ENTER_SINK_METHOD(SyncFile_method_idx);
  tsp00_VfReturn_Param Error;
  if (NULL != lpFileName) {
    eo15SyncNamedMessageFile(lpFileName, Error);
  }
  else {
    eo15SyncMessageFile(FileHandle, Error);
  }
  if (vf_ok == Error) {
    *lpb_error = e_ok;
  }
  else {
    char buf[132];
    if (NULL != lpFileName)
      sprintf(&buf[0],"SyncFile %.100s err: %d ", lpFileName, Error );
    else
      sprintf(&buf[0],"SyncFile hd: %d err: %d ", FileHandle, Error );
    MSGD ((ERR_DCOM_DUMP_INFO,buf));
    *lpb_error = a071_return_code(e_hostfile_error, sqlm_internal);
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetDateTime(tsp00_Date* Date,
                                          tsp00_Time* Time)
{
  ENTER_SINK_METHOD(GetDateTime_method_idx);
  vdattime(*Date, *Time);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetClock(tsp00_Int4* Sec,
                                       tsp00_Int4* MSec)
{
  ENTER_SINK_METHOD(GetClock_method_idx);
  vclock(Sec, MSec);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DropVersion(unsigned char *lpVersionContext,
                                          short         *lpb_error)
{
  ENTER_SINK_METHOD(DropVersion_method_idx);
  tgg00_BasisError error = 0;
  ak341DropVersion (*this->lpSessionContext,
    *REINTERPRET_CAST(tgg01_OmsVersionContext*, lpVersionContext),
    error);
  *lpb_error = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::CreateSchema(const OmsTypeWyde *SchemaName,
                                          tsp00_Int4         *SchemaId,
                                          short              *lpb_error)
{
  ENTER_SINK_METHOD(CreateSchema_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  a34CreateSchema (context, 
    *REINTERPRET_CAST(tak_WydeArray*, CONST_CAST(OmsTypeWyde*, SchemaName)), 
    *SchemaId);
  *lpb_error = context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ExistsSchema(const OmsTypeWyde   *SchemaName,
                                          tsp00_Int4           *SchemaId,
                                          short                *lpb_error)
{
  ENTER_SINK_METHOD(ExistsSchema_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  a34ExistsSchema (context,
    *REINTERPRET_CAST(tak_WydeArray*, CONST_CAST(OmsTypeWyde*, SchemaName)),
    *SchemaId);
  *lpb_error = context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DropContainer(OmsSchemaHandle schema,
                                         const GUID&        guid,
                                         OmsContainerNo     cno,
                                         tgg00_BasisError&  e)
{
  ENTER_SINK_METHOD(DropContainer_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  if (LVC_ContainerManager::DropContainer (context, schema, cno, *REINTERPRET_CAST(const Catalog_GUID*, &guid)))
  {
      e = 0;
  }
  else
  {
      e = context.ReturnCode();
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::DropSchema(tsp00_Int4 SchemaId,
                                         short      *lpb_error)
{
  ENTER_SINK_METHOD(DropSchema_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  a34DropSchema (context, SchemaId); 
  *lpb_error =  context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetSchemaName(tsp00_Int4 SchemaId,
                                         OmsTypeWyde   *SchemaName,
                                         short         *lpb_error)
{
  ENTER_SINK_METHOD(GetSchemaName_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  a34GetSchemaName (context,
    SchemaId,
    *REINTERPRET_CAST(tak_WydeArray*, SchemaName));
  *lpb_error =  context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::LoadLibraryEx(const char  *path,
                                            bool        loadGlobal,
                                            HANDLE      *hd,
                                            tsp00_ErrText &errtext,
                                            int         errtext_size)
{
  ENTER_SINK_METHOD(LoadLibraryEx_method_idx);
  LVC_LoadLibrary(path, loadGlobal, hd, errtext, errtext_size);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::Reschedule() /* PTS 1107849 */
{
  ENTER_SINK_METHOD(Reschedule_method_idx);
  vsleep(this->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00, 0);
  LEAVE_SINK_METHOD(S_OK);
}

/*----- PTS 1132163, PG -----------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::Sleep(const OmsTypeUInt1 sleepTime)
{
  ENTER_SINK_METHOD(Sleep_method_idx);
  tsp00_Int2 duration = (tsp00_Int2)sleepTime;
  vsleep(this->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00, duration);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::FreeLibrary(HANDLE      hd,
                                          bool        &ok,
                                          tsp00_ErrText &errtext,
                                          int         errtext_size)
{
  ENTER_SINK_METHOD(FreeLibrary_method_idx);
  LVC_FreeLibrary(hd, &ok, errtext, errtext_size);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/
// PTS 1124356 FF 2003-09-23 new FileDir
STDMETHODIMP CIliveCacheSink::GetContainerInfo(OMS_ContainerHandle &containerHandle,
            GUID&            guid,
            OmsSchemaHandle& schema,
            OmsContainerNo&  cno,
            short&           e)
{
  ENTER_SINK_METHOD(GetContainerInfo_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  FileDir_FileNo fileNo(*reinterpret_cast<tsp00_C8*>(&containerHandle));
  Catalog_ObjectId SchemaObjectId;
  LVC_ContainerManager::GetContainerInfo (context, fileNo, *REINTERPRET_CAST(Catalog_GUID*, &guid), 
      schema, cno, SchemaObjectId);
  e = context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::IsObjHistoryUsed(
            const SAPDB_UInt8& oid,
      			bool               ignoreVersions,        /* PTS 1130354 */
            bool&              isUsed,
            tgg00_BasisError&  e,
      			OmsTypeUInt8       &OldestTransNo)        /* PTS 1130354 */
{
  ENTER_SINK_METHOD(IsObjHistoryUsed_method_idx);
  tgg00_BasisError error = 0;
  tgg01_ContainerId containerId;
  containerId.fileObjFileType_gg00().becomes(oftUnknownType_egg00);

  SAPDB_UInt8 &tmpOldestTransNo = *(SAPDB_UInt8 *)&OldestTransNo;
  ak341IsObjHistoryUsed (*this->lpSessionContext,
    containerId,
    *REINTERPRET_CAST(const tgg92_KernelOid*, &oid),
		ignoreVersions,								/* PTS 1130354 */
    isUsed,
    error,
		tmpOldestTransNo);					      /* PTS 1130354 */
  e = (error == 0 ? 0 : a071_return_code(error, sqlm_internal));   // PTS 1124935
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::Signal(tsp00_TaskId taskId, tsp00_Bool ok) // PTS 1110315
{
  ENTER_SINK_METHOD(Signal_method_idx);
  vsignal(taskId, ok);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::Wait(tsp00_Bool& ok) // PTS 1110315
{
  ENTER_SINK_METHOD(Wait_method_idx);
  vwait(this->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00, ok);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ExistsContainer(
    tsp00_Int4            schemaId,
    const GUID           &guid,
    OmsContainerNo        containerNo,
    size_t                objectSize,
    size_t                keyLen,
    tgg01_ContainerId    &containerId,
    bool                 &useCachedKeys,
    bool                 &partitionedKeys,
    short                &e)  // PTS 1110334
{
  ENTER_SINK_METHOD(ExistsContainer_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  if (LVC_ContainerManager::ExistsContainer (context, schemaId, containerNo, *REINTERPRET_CAST(const Catalog_GUID*, &guid), objectSize,
      containerId, useCachedKeys))
  {
      e = 0;
  }
  else
  {
    e = (0 == context.ReturnCode()) ? e_OMS_sysinfo_not_found : context.ReturnCode();
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::LockContainer(
  const tgg00_FileId    &fileId,
  short                 &error)  
{
  ENTER_SINK_METHOD(LockContainer_method_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  ak341LockContainer(context, fileId);
  error = context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetSessionHeapUsage(
            /* [out] */ tsp00_8ByteCounter& heapUsage)
{
  heapUsage = this->lpSessionContext->a_allocatorInfo.ai_alloc_sys.ami_Allocated +
    this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_Allocated;
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SimCtlDumpToHtml(
	OmsHandle     &handle,
	SAPDB_UInt8   obj,
	class OmsSimDumpCollector &str)
{
	// TODO
	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SimCtlGetHTMLRef(
	class OmsHandle &handle,
	SAPDB_UInt8     obj,
	const char     **data,
	int              flags)
{
	*data = "";
	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SimCtlSetObjectName(
	OmsHandle   &handle,
	SAPDB_UInt8 obj,
	const char  *name)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SimCtlGetObjectName(
	OmsHandle   &handle,
	SAPDB_UInt8 obj,
	const char **name)
{
	*name = NULL;
	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SimCtlDestroyHandleCB(
	class OmsHandle				&handle,
	struct IliveCacheSink		**session)
{
	return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::AttachSequence(const tsp00_KnlIdentifier& sequenceName,
                                             tgg00_Surrogate&           sequenceId,
                                             tgg00_SeqInfo&             sequenceInfo,
                                             short&                     e)
{
    ContextScope contextScope(*this->lpSessionContext);
    a34Sequence (*this->lpSessionContext, sequenceName, sequenceId, sequenceInfo, e); // PTS 1137184
    return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::NextVal(const tgg00_Surrogate& sequenceId,
                                      tgg00_SeqInfo&         sequenceInfo,
                                      tsp00_Uint4&           nextVal,
                                      short&                 sqlError)
{
    ENTER_SINK_METHOD(NextVal_idx);
    ContextScope contextScope(*this->lpSessionContext);
    sqlError = 0;
    nextVal = ak341NextVal (contextScope.GetContext(), sequenceId, sequenceInfo, sqlError);
    LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::IsInRegion(
                                         int    regionId,
                                         bool  &inRegion,
                                         short &error) 
{
    if (regionId < OMS_SYS_REGION)
    {
        inRegion = m_inRegion[regionId];
        error    = e_ok;
    }
    else
    {
        inRegion = false;
        error    = e_OMS_not_implemented;
        //throw DbpError(DbpError::DB_ERROR, e_OMS_not_implemented, __FILE__, __LINE__);
    }
    return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::IsUnicodeInstance(bool& isUnicodeInstance, short& error)
{
    ENTER_SINK_METHOD(IsUnicodeInstance_idx);
    error = e_ok;
    isUnicodeInstance = ((pasbool) 0 != a34IsUnicodeLivecacheUser(*this->lpSessionContext));
    LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP_(int) CIliveCacheSink::GetSinkType()
{
  return 0;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::GetContainerId(
    tgg01_ContainerId              *pContainerId,
    const tgg01_OmsVersionContext  *pVersionContext,
    tsp00_Int4                      noOfOid,
    const SAPDB_UInt8              *pOid,
    tgg00_BasisError               *pDBError)
{
  ENTER_SINK_METHOD(GetContainerId_idx);
  tgg00_BasisError DBError = 0;
  for (int i=0; i<noOfOid; ++i){
    if (i>0
       &&    reinterpret_cast<const tgg92_KernelOid*>(&pOid[i])->gg92GetPno()
          == reinterpret_cast<const tgg92_KernelOid*>(&pOid[i-1])->gg92GetPno()){
      pContainerId[i] = pContainerId[i-1];
    }
    else {
      ak341GetContainerId (this->lpSessionContext,
                           pContainerId[i],
                           pVersionContext,
                           *reinterpret_cast<const tgg92_KernelOid*>(&pOid[i]),
                           DBError);
      if (DBError != e_ok){
        pDBError[i] = a071_return_code(DBError, sqlm_internal);   // PTS 1124935
        DBError     = 0;
      }
      else {
        pDBError[i] = 0;
      }
    }
  }
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::SetTransactionComment(tsp00_Int2         commentLength,
                                                    const OmsTypeWyde *pComment,
                                                    short             &Error) 
{
  ENTER_SINK_METHOD(SetTransactionCommentMethod_idx);
  ContextScope contextScope(*this->lpSessionContext);
  SQLMan_Context& context = contextScope.GetContext();
  ak341SetTransactionComment (context,
      commentLength,
      REINTERPRET_CAST(const tak_WydeArray*, pComment)); 
  Error =  context.ReturnCode();
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictInsert(
    const OmsVersionId  &versionId,
    const OMS_Context   *pVersionContext,
    tgg00_BasisError    &error)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictFind(
    const OmsVersionId   &versionId,
    OMS_Context         **ppVersionContext)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictDrop(
    const OmsVersionId  &versionId,
    tgg00_BasisError    &error)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictShutdown()
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictUnloadOldestVersion(
  bool &unloaded)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictMarkNotUnloadable(
    OMS_Context *pVersionContext)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictMarkUnloadable(
    OMS_Context *pVersionContext)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictCreateIter(
  OMS_LockMode   lockMode,
  void         **ppIter,
  OMS_Context  **ppContext)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictGetNext(
  void         *pIter,
  OMS_Context **ppContext)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::VersionDictDestructIter(
  void  *pIter)
{
  return S_OK;
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::IsTestKnlBadAllocActive(bool &active)
{
  ENTER_SINK_METHOD(IsTestKnlBadAllocActiveMethod_idx);
  active = (lpSessionContext->a_allocatorInfo.ai_badAllocCountDown == -2);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

STDMETHODIMP CIliveCacheSink::ResetMonitor()
{
  ENTER_SINK_METHOD(ResetMonitorMethod_idx);
  a42ResetOmsMonitor(*lpSessionContext);
  LEAVE_SINK_METHOD(S_OK);
}

/*---------------------------------------------------------------------------*/

inline void eo38_EvalTaskSelfIOInfo(const tsp92_RteTaskSelfIOInfo& currInfo, tsp92_RteTaskSelfIOInfo& resInfo)
{
  resInfo.Cnt         = currInfo.Cnt        - resInfo.Cnt;
  resInfo.Pages       = currInfo.Pages      - resInfo.Pages;
  resInfo.AvgAbsTime  = currInfo.AvgAbsTime - resInfo.AvgAbsTime;
  resInfo.TotAbsTime  = currInfo.TotAbsTime - resInfo.TotAbsTime;
}

inline void eo38_EvalTaskDEVIOIOInfo(const tsp92_RteTaskDEVIOInfo& currInfo, tsp92_RteTaskDEVIOInfo& resInfo)
{
  resInfo.Cnt         = currInfo.Cnt - resInfo.Cnt;
  resInfo.Pages       = currInfo.Pages - resInfo.Pages;
  resInfo.AvgRelTime  = currInfo.AvgRelTime;
  resInfo.TotRelTime  = currInfo.TotRelTime - resInfo.TotRelTime;
  resInfo.AvgAbsTime  = currInfo.AvgAbsTime;
  resInfo.TotAbsTime  = currInfo.TotAbsTime - resInfo.TotAbsTime;
}

inline void eo38_EvalTaskStateInfo(const tsp92_RteTaskStateInfo& currInfo, tsp92_RteTaskStateInfo& resInfo)
{
  resInfo.Cnt         = currInfo.Cnt - resInfo.Cnt;
  resInfo.AvgRelTime  = currInfo.AvgRelTime;
  resInfo.TotRelTime  = currInfo.TotRelTime - resInfo.TotRelTime;
  resInfo.AvgAbsTime  = currInfo.AvgAbsTime;
  resInfo.TotAbsTime  = currInfo.TotAbsTime - resInfo.TotAbsTime;
}


HRESULT STDMETHODCALLTYPE CIliveCacheSink::EvalCallStatistics(
            bool            AllocatorOnly,
            OmsMonitorData& MonitorData)
{
  if (NULL != lpCallTaskInfo)
  {
    if (!AllocatorOnly)
    {
      tsp92_RteTaskDetailsInfo taskInfo;
      vGetRteInfoTaskDetails ( this->lpSessionContext->a_transinf.tri_trans.trTaskId_gg00, &taskInfo);
      MonitorData.m_cntDispatcher   = (OmsTypeUInt4)(taskInfo.DispatcherCnt - lpCallTaskInfo->DispatcherCnt);
      MonitorData.m_cntSelfSuspends = (OmsTypeUInt4)(taskInfo.TaskSelfSuspCnt - lpCallTaskInfo->TaskSelfSuspCnt);
      MonitorData.m_OmsLVCBaseCalls = (OmsTypeInt8) (taskInfo.OMSCallbackCount - lpCallTaskInfo->OMSCallbackCount);
      /*-------------------------------------- io statistics ----------------------------------------*/
      eo38_EvalTaskSelfIOInfo ( taskInfo.SelfIORead, lpCallTaskInfo->SelfIORead );
      eo38_EvalTaskSelfIOInfo ( taskInfo.SelfIOWrite, lpCallTaskInfo->SelfIOWrite );
      eo38_EvalTaskDEVIOIOInfo( taskInfo.DEVIORead, lpCallTaskInfo->DEVIORead );
      eo38_EvalTaskDEVIOIOInfo( taskInfo.DEVIOWrite, lpCallTaskInfo->DEVIOWrite );
      MonitorData.m_cntIO           = (OmsTypeUInt4) (lpCallTaskInfo->SelfIORead.Cnt +
        lpCallTaskInfo->SelfIOWrite.Cnt +
        lpCallTaskInfo->DEVIORead.Cnt +
        lpCallTaskInfo->DEVIOWrite.Cnt);
      MonitorData.m_IORelTime       = (OmsTypeInt4)  (lpCallTaskInfo->SelfIORead.TotAbsTime +
        lpCallTaskInfo->SelfIOWrite.TotAbsTime +
        lpCallTaskInfo->DEVIORead.TotRelTime +
        lpCallTaskInfo->DEVIOWrite.TotRelTime);
      MonitorData.m_IOAbsTime       = (OmsTypeInt4)  (lpCallTaskInfo->SelfIORead.TotAbsTime +
        lpCallTaskInfo->SelfIOWrite.TotAbsTime +
        lpCallTaskInfo->DEVIORead.TotAbsTime +
        lpCallTaskInfo->DEVIOWrite.TotAbsTime);
      /*------------------------------ receive/reply statistics ------------------------------------*/
      eo38_EvalTaskStateInfo  ( taskInfo.ReplyReceive, lpCallTaskInfo->ReplyReceive );
      eo38_EvalTaskStateInfo  ( taskInfo.ReceiveReply, lpCallTaskInfo->ReceiveReply );
      MonitorData.m_cntReplyReceive     = (OmsTypeUInt4) lpCallTaskInfo->ReplyReceive.Cnt;
      MonitorData.m_ReplyReceiveRelTime = (OmsTypeInt4)  lpCallTaskInfo->ReplyReceive.TotRelTime;
      MonitorData.m_ReplyReceiveAbsTime = (OmsTypeInt4)  lpCallTaskInfo->ReplyReceive.TotAbsTime;
      MonitorData.m_cntReceiveReply     = (OmsTypeUInt4) lpCallTaskInfo->ReceiveReply.Cnt;
      MonitorData.m_ReceiveReplyRelTime = (OmsTypeInt4)  lpCallTaskInfo->ReceiveReply.TotRelTime;
      MonitorData.m_ReceiveReplyAbsTime = (OmsTypeInt4)  lpCallTaskInfo->ReceiveReply.TotAbsTime;
      eo38_EvalTaskStateInfo  ( taskInfo.Vsuspend, lpCallTaskInfo->Vsuspend );
      /*---------------------------------- vwait statistics ----------------------------------------*/
      eo38_EvalTaskStateInfo  ( taskInfo.Vwait, lpCallTaskInfo->Vwait );
      MonitorData.m_cntWaits            = (OmsTypeUInt4) lpCallTaskInfo->Vwait.Cnt;
      MonitorData.m_WaitsRelTime        = (OmsTypeInt4) lpCallTaskInfo->Vwait.TotRelTime;
      MonitorData.m_WaitsAbsTime        = (OmsTypeInt4) lpCallTaskInfo->Vwait.TotAbsTime;
      /*---------------------------------- vsleep statistics ---------------------------------------*/
      eo38_EvalTaskStateInfo  ( taskInfo.Vsleep, lpCallTaskInfo->Vsleep );
      MonitorData.m_cntSleep            = (OmsTypeUInt4) lpCallTaskInfo->Vsleep.Cnt;
      MonitorData.m_SleepRelTime        = (OmsTypeInt4) lpCallTaskInfo->Vsleep.TotRelTime;
      MonitorData.m_SleepAbsTime        = (OmsTypeInt4) lpCallTaskInfo->Vsleep.TotAbsTime;
    }
    /*-------------------------------- allocator statistics --------------------------------------*/
    MonitorData.m_cntOperatorNew    = (OmsTypeInt8) this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_MallocCnt;
    if (MonitorData.m_cntOperatorNew > 0)
    {
        MonitorData.m_maxChunkAllocated = (OmsTypeUInt4) this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_MallocMax;
        MonitorData.m_minChunkAllocated = (OmsTypeUInt4) this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_MallocMin;
        MonitorData.m_avgChunkAllocated = (OmsTypeUInt4) (this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_MallocSum /
            this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_MallocCnt);
    }
    else
    {
        MonitorData.m_maxChunkAllocated = 0;
        MonitorData.m_minChunkAllocated = 0;
        MonitorData.m_avgChunkAllocated = 0;
    }
    MonitorData.m_cntOperatorDelete = (OmsTypeInt8) this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_FreeCnt;
    if (MonitorData.m_cntOperatorDelete > 0)
    {
        MonitorData.m_maxChunkDeleted   = (OmsTypeUInt4) this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_FreeMax;
        MonitorData.m_minChunkDeleted   = (OmsTypeUInt4) this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_FreeMin;
        MonitorData.m_avgChunkDeleted   = (OmsTypeUInt4) (this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_FreeSum /
            this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_FreeCnt);
    }
    else
    {
        MonitorData.m_maxChunkDeleted   = 0;
        MonitorData.m_minChunkDeleted   = 0;
        MonitorData.m_avgChunkDeleted   = 0;
    }
    MonitorData.m_maxHeapUsage = (OmsTypeInt8) this->lpSessionContext->a_allocatorInfo.ai_HeapUsageMax;
    MonitorData.m_memoryDelta = (OmsTypeInt64) (this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_MallocSum
         + this->lpSessionContext->a_allocatorInfo.ai_alloc_sys.ami_MallocSum
         - this->lpSessionContext->a_allocatorInfo.ai_alloc_usr.ami_FreeSum
         - this->lpSessionContext->a_allocatorInfo.ai_alloc_sys.ami_FreeSum);
    MonitorData.m_cntOutOfMemory = this->lpSessionContext->a_allocatorInfo.ai_badAllocCount; // PTS 1126797
  }
  return S_OK;
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
