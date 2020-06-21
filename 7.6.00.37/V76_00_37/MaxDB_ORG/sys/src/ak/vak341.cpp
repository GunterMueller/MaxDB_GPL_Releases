/* @lastChanged: "1999-04-21  15:46"

  * @filename:   vak341.cpp
  * @purpose:    "Kernel Sink"
  * @release:    7.2.0.0
  * @see:        ""
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


/* ========================================================================== */
/* ==========           REPLACEMENTS BY THE PREPROCESSOR            ========= */
/* ========================================================================== */

#include "ggg01.h" 
#include "heo00.h" 
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hkb04.h"
#include "hbd01.h"
#include "hbd02.h"
#include "hbd04.h"
#include "hak01.h"
#include "hak34.h"
#include "hak341.h"
#include "hak37.h"
#include "vak001.h"
#include "gak40.h"
#include "hak40.h"
#include "hak07.h"
#include "hgg01.h"
#include "hgg01_1.h"
#include "hgg01_3.h"
#include "Oms/OMS_Defines.h"
#include "hak05.h"
#include "hak51.h"
#include "heo01.h"
#include "heo58.h"
#include "heo66.h"
#include "heo78.h"
#include "heo02x.h"
#include "heo38.h"
#include "heo670.h"
#include "ilivecachesink.h"  /* no check */
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "hgg11.h"
#include "hgg04.h"
#include "heo15.h"
#include "gsp03_1.h"
#include "hsp26.h"
#include "hsp40.h"
#include "hsp77.h"
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "liveCache/MemoryManagement/LVCMem_EmergencyAllocator.hpp"
#include "liveCache/LVC_KernelInterfaceInstance.hpp"
#include "liveCache/LVC_LibOmsInterface.hpp"
#include "liveCache/LVC_LoadLibrary.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp" 
#include "Oms/OMS_DumpInterface.hpp"
#include "Oms/OMS_TraceInterface.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SystemViews/SysView_PseudoLongHandler.hpp"
#include "PacketInterface/PIn_Packet.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */


class Dump_cak341 : public OMS_DumpInterface
{
public :
  Dump_cak341(
    void*           session,
    tgg00_VfFileref& f,
    tsp00_Page&       buf, 
    tsp00_Int4&     pno, 
    int&            pos,
    tsp00_VfReturn&  vfReturn,
    tsp00_ErrText&    errTxt,
    bool            enterRegions) 
    :
  m_file(f),
  m_buf(buf),
  m_session(session),
  m_pos(pos),
  m_pno(pno),
  m_headerPos(-1),
  m_vfReturn(vfReturn),
  m_errTxt(errTxt),
  m_sync(enterRegions)
  {
  }
  ~Dump_cak341()
  {
    this->Flush();
  }
  virtual void* GetOmsSession();
  virtual void SetDumpLabel (const char* label, int dumpType);
  virtual void Dump (void* p, int len);
  virtual bool Synchronize() const;
  void    TriggerDump();
private :
  void Flush();
  tgg00_VfFileref& m_file;
  tsp00_Buf&      m_buf;
  void*           m_session;
  tsp00_C8        m_label;
  tsp00_Int2      m_dumpType;
  int&            m_pos;
  int             m_headerPos;
  tsp00_Int4&     m_pno;
  tsp00_VfReturn&  m_vfReturn;
  tsp00_ErrText&    m_errTxt;
  bool            m_sync;
};

/*-----------------------------------------------------------------------------------*/


class Trace_cak341 : public OMS_TraceInterface
{
public :
  Trace_cak341(const char* fn); 
  virtual void Trace( const char*fmt, ... );
private :
  tsp00_Int4 m_file;
};

typedef bool (*QUALTEST) (unsigned char*, void*);

/* ========================================================================== */
/* ==========              DECLARATION OF FUNCTIONS                 ========= */
/* ========================================================================== */

bool ak341QualificationTest (void* QualFunc,
                   void*          QualValues,
                   unsigned char* Obj);

void ak341SessionDump(tsp00_TaskId,tsp00_Int4,void*);

/* ========================================================================== */
/* ==========                     Globals                           ========= */
/* ========================================================================== */

static OMS_LibOmsInterface* ak341LibOmsInterfacePtr = 0;
static Trace_cak341*        ak341pTrace = (Trace_cak341*) 0;
#if defined(OMSTST)
static bool                   ak341NormalOmsTerminate = false;
#endif
   
/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */
   
#if defined(OMSTST)
externC
bool ak341NormalOmsTerminateHandling()
{
    return ak341NormalOmsTerminate;
}
#endif

/*-----------------------------------------------------------------------------------*/

externCpp void
ak341CancelOmsVersion (tgg91_TransNo &ConsistViewNo)
{
    ak341LibOmsInterfacePtr->CancelVersion(ConsistViewNo);
};

/*-----------------------------------------------------------------------------------*/

externCpp void
ak341CreateVersion (tak_all_command_glob&     acv,
                     tgg01_OmsVersionContext& versionContext,
                     tgg00_BasisError&        e)
{
  struct tSurrogateRecord {
    tsp00_C2   filler;
    tsp00_C2   hi;
    tsp00_Int4 lo;
  };

  union {
     tgg00_Surrogate surrogate;
     tSurrogateRecord surrogateRec;
  } versionSurrogate;

  e = e_ok;
  // ++++ CHECK IF THE TRANSACTION CHANGED ANYTHING ++++
  // IF YES THEN e = e_transaction_end_required;
  // PTS 1111821 UH 2001-11-20
  // k50my_state(ObjLogExists) is removed and must be exchanged with an equivalent check from the oms-layer
  a05surrogate_get (acv, versionSurrogate.surrogate);
  g04build_temp_tree_id (versionContext.ovc_tree, acv.a_transinf.tri_trans);
  versionContext.ovc_tree.fileSession_gg00().gg90SetNil();    
  versionContext.ovc_tree.fileType_gg00().addElement(ftsShared_egg00);
  versionContext.ovc_tree.fileType_gg00().addElement(ftsConcurrent_egg00);
  versionContext.ovc_tree.fileTfn_gg00().becomes(tfnTempOms_egg00);  // PTS 1109375 TS 2003-02-24
  versionContext.ovc_tree.fileTfnTemp_gg00().becomes(ttfnOmsVersion_egg00);
  versionContext.ovc_tree.fileLevel_gg00()   = versionSurrogate.surrogateRec.hi;
  versionContext.ovc_tree.fileTempCnt_gg00() = versionSurrogate.surrogateRec.lo;
  versionContext.ovc_index_tree              = versionContext.ovc_tree;
  versionContext.ovc_key                     = 0;
  b01tcreate_file (acv.a_transinf.tri_trans, versionContext.ovc_tree);
  e = acv.a_transinf.tri_trans.trError_gg00;
  if (e_ok == e) {
     versionContext.ovc_index_tree.fileTfnTemp_gg00().becomes(ttfnOmsVersionIndex_egg00);
     b01tcreate_file (acv.a_transinf.tri_trans, versionContext.ovc_index_tree);
     e = acv.a_transinf.tri_trans.trError_gg00;
  }
}

/*-----------------------------------------------------------------------------------*/

void
ak341DropVersion (tak_all_command_glob&    acv,
                  tgg01_OmsVersionContext& VersionContext,
                  tgg00_BasisError&        e)
{
  
  ak341EndConsistentView (&acv, &VersionContext.ovc_trans_version, &e);
  if (e_ok == e) {
    b01destroy_file (acv.a_transinf.tri_trans, VersionContext.ovc_tree);
    e = acv.a_transinf.tri_trans.trError_gg00;
  }
  if (e_ok == e) {
    b01destroy_file (acv.a_transinf.tri_trans, VersionContext.ovc_index_tree);
    e = acv.a_transinf.tri_trans.trError_gg00;
  }
}

/*-----------------------------------------------------------------------------------*/

externC void
ak341EndConsistentTrans (tgg00_TransContext &Trans)
{
  Trans.trError_gg00 = e_ok;
  if (!Trans.trConsistView_gg00.gg90IsNil()) {
    kb04EndConsistentTrans (Trans);
  }
}

/*-----------------------------------------------------------------------------------*/

externC void
ak341GetObjFileInfo (tgg00_TransContext  &Trans,
					 tgg01_ContainerId   &ObjFileId,
					 tsp00_Int4          &NumAllPages,
					 tsp00_Int4          &NumFreePages,
					 tsp00_Int4          &NumEmptyPages,
                     tsp00_Int4          &NumKeyLeaves,
					 tsp00_Int4          &Occupancy)
{				
	Trans.trError_gg00 = e_ok;

	/* change request 1000139, T.A. 14.10.1998 */
	/* PTS            1107153, A.K. 17.07.2000 */
	bd04GetObjFileStatistics (Trans, ObjFileId, NumAllPages, 
		NumFreePages, NumEmptyPages, NumKeyLeaves, Occupancy);    
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341HeapCallStackMonitoring(int level)
{
  LVCMem_Allocator::LVCAllocatorInstance().SetCallStackMonitoring(level);
  SQLMan_Context::GetContext()->GetAllocator().SetCallStackMonitoring(level);
}

/*-----------------------------------------------------------------------------------*/

externC tsp00_Bool
ak341GetMonitorInfo (tak_all_command_glob& acv,
             tsp00_Addr&                   handle,
             tsp00_C16&                    iid,
             tsp00_Int4&                   dispid,
             tgg01_COMMonitorInfo&         monInfo)
{
  return ak341LibOmsInterfacePtr->GetMonitorInfo (
    REINTERPRET_CAST(IliveCacheSink*, &acv.a_kernel_sink), handle, iid, dispid, monInfo);
}

/*-----------------------------------------------------------------------------------*/

externC int
ak341GetOmsUnloadedVersions()
{
    return LVC_KernelInterfaceInstance::GetOmsVersionUnloads();
}

/*-----------------------------------------------------------------------------------*/

externC 
void ak341ReleaseEmergencyAllocator(tsp00_TaskId taskId)
{
    LVCMem_EmergencyAllocator::Instance().Release(taskId);
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341ResetMonitor(tak_all_command_glob& acv)
{
  ak341LibOmsInterfacePtr->ResetMonitor(REINTERPRET_CAST(IliveCacheSink*, &acv.a_kernel_sink));
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341ResetOmsVersionUnloadCounter()
{
  LVC_KernelInterfaceInstance::ResetOmsVersionUnloadCounter();
}

/*-----------------------------------------------------------------------------------*/

static 
void ak341SetTraceLevel (const char* Level)
{
    Msg_List msgList;
    RTEConf_Parameter::Integer integerValue;
    if((RTEConf_ParameterAccess::Instance())->GetInteger((RTEConf_Parameter::Name) Level, integerValue, msgList))
    {
        if (integerValue > 0)
        {
            ak341LibOmsInterfacePtr->SetTraceLevel(&Level[6], true);
        }
    }
}

/*-----------------------------------------------------------------------------------*/

externC 
void ak341LoadLibOms(tak_all_command_glob& acv) {
    tsp00_Bool        ok;
    tsp00_Pathc       szInstPath;
    tsp01_RteError    rteError;
    teo78_ModuleName  OmsLibPath;
    tsp00_ErrText       errtext;
    HANDLE            hd = 0;

#if defined(WIN32)
    ok = sqlGetDbrootPgmPath (szInstPath, TERM_WITH_DELIMITER_EO01, &rteError );
#else
    ok = sqlGetDbrootLibPath (szInstPath, TERM_WITH_DELIMITER_EO01, &rteError );
#endif
    TheLiveCacheAllocator.Create(g01oms_heap_count(), g01oms_heap_blocksize(), g01oms_heap_limit(), true);
    if (ok) 
    {
        strcpy(OmsLibPath, szInstPath);
#   if defined(OMSTST)
        char delimiter[2];
        delimiter[0] = szInstPath[strlen(szInstPath) -1];
        delimiter[1] = 0;
        strcat(OmsLibPath, "test");
        strcat(OmsLibPath, &delimiter[0]);
#   endif
        strcat(OmsLibPath, "liboms" );
        Kernel_OpInfo(csp3_n_obj) << OmsLibPath; 
        LVC_LoadLibrary(OmsLibPath, true, &hd, errtext, sizeof(tsp00_ErrText)-1);
    }
    if (0 == hd) {
        a07_b_put_error (acv, e_no_coclass_obj_found, 1);
    }
    else {
        OMS_ProcGetInterfacePtr I = (OMS_ProcGetInterfacePtr) sqlGetProcAddress(hd, (char*) "co10_GetInterface",
            REINTERPRET_CAST(char*, &errtext[0]), sizeof(errtext));
        if (NULL == I) {
            a07_b_put_error (acv, e_invalid, 1);
            return;
        }
        ak341LibOmsInterfacePtr = (*I)();
        LVC_LibOmsInterface::SetInterface(ak341LibOmsInterfacePtr);
        ak341LibOmsInterfacePtr->AdviseKernelInterface(LVC_KernelInterfaceInstance::Instance(), 
            true, /*isKernel*/
            false /*isUDEServer*/);
#if defined(OMS_TRACE)
        ak341pTrace = new Trace_cak341("Trace.prt");
        ak341LibOmsInterfacePtr->AdviseTracer(ak341pTrace);
#endif
        tak_kernel_sink* pSink = &acv.a_kernel_sink;
        ak341LibOmsInterfacePtr->InitLibOms(REINTERPRET_CAST(IliveCacheSink*, pSink));
        ak341SetTraceLevel ("TRACE_OMS_CONTAINERDIR");
        ak341SetTraceLevel ("TRACE_OMS_CONTEXT");
        ak341SetTraceLevel ("TRACE_OMS_ERROR");
        ak341SetTraceLevel ("TRACE_OMS_FLUSHCACHE");
        ak341SetTraceLevel ("TRACE_OMS_INTERFACE");
        ak341SetTraceLevel ("TRACE_OMS_KEY");
        ak341SetTraceLevel ("TRACE_OMS_KEYRANGE");
        ak341SetTraceLevel ("TRACE_OMS_LOCK");
        ak341SetTraceLevel ("TRACE_OMS_MEMORY");
        ak341SetTraceLevel ("TRACE_OMS_NEWOBJ");
        ak341SetTraceLevel ("TRACE_OMS_SESSION");
        ak341SetTraceLevel ("TRACE_OMS_STREAM");
        ak341SetTraceLevel ("TRACE_OMS_VAROBJECT");
        ak341SetTraceLevel ("TRACE_OMS_VERSION");
    }
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341OmsHeapInfo(tak_all_command_glob& acv, tsp00_8ByteCounter& totalHeap,
                                                 tsp00_8ByteCounter& freeSpaceInFreeChunks,
                                                 tsp00_8ByteCounter& reserved,
                                                 tsp00_8ByteCounter& mallocated,
                                                 tsp00_8ByteCounter& emergencySize,
                                                 tsp00_8ByteCounter& emergencyInUse,
                                                 tsp00_8ByteCounter& emergencyMaxUsed)
{
    SAPDB_ULong BytesUsed;
    SAPDB_ULong MaxBytesUsed;
    SAPDB_ULong BytesControlled;
    SAPDB_ULong EmergencySize; 
    SAPDB_ULong EmergencyInUse; 
    SAPDB_ULong EmergencyMaxUsed;  

    TheLiveCacheAllocator.CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
    totalHeap             = (tsp00_8ByteCounter) BytesUsed;
    freeSpaceInFreeChunks = 0;
    reserved              = (tsp00_8ByteCounter) BytesControlled;
    mallocated            = 0;
    TheLiveCacheAllocator.GetEmergencyAllocatorInfo(EmergencyInUse, EmergencySize, EmergencyMaxUsed);
    emergencySize         = (tsp00_8ByteCounter) EmergencySize;
    totalHeap            += emergencySize;
    reserved             += emergencySize;
    emergencyInUse        = (tsp00_8ByteCounter) EmergencyInUse;
    emergencyMaxUsed      = (tsp00_8ByteCounter) EmergencyMaxUsed;
}

/*-----------------------------------------------------------------------------------*/
// PTS 1110287
externC
void ak341OmsInspectTimeout()
{
    if (ak341LibOmsInterfacePtr)
    {
        ak341LibOmsInterfacePtr->InspectTimeout();
    }
}

/*-----------------------------------------------------------------------------------*/
// PTS 1110287
externC
tsp00_Bool ak341NextOmsLockObjInfo(void** pHandle, tgg01_OmsLockInfo& lockInfo)
{
  return ak341LibOmsInterfacePtr->NextOmsLockObjInfo(pHandle, lockInfo);
}

/*-----------------------------------------------------------------------------------*/

tsp00_Uint4 ak341NextVal (tak_all_command_glob&  acv,
                          const tgg00_Surrogate& sequenceId,
                          tgg00_SeqInfo&         sequenceInfo,
                          short&                 sqlError)

{
    tsp00_NumError res;
    tsp00_C20       number;
    int           length;
    tsp00_Int4    nextVal;

    acv.a_returncode = 0;

    a05_nextval_get (acv, mm_nil, sequenceId, sequenceInfo, &number[0], 1, length);

    sqlError = acv.a_returncode;
    if (0 == sqlError)
    {
        s40gluns (&number[0], 2, (length - 2) * 2, nextVal, res);
    }
    return nextVal;
}

/*-----------------------------------------------------------------------------------*/

externC
tsp00_Bool ak341NextVersion (tak_all_command_glob& acv,
                       tsp00_Bool          isFirstCall,
                       tsp00_C24&          versionId,
                       tsp00_Date&         createDate,
                       tsp00_Time&         createTime,
                       tsp00_Date&         openDate,
                       tsp00_Time&         openTime,
                       tgg91_TransNo&      consistentView,
                       tsp00_Bool&         isMarked,
                       tsp00_Bool&         isOpen,
                       tsp00_Bool&         isUnloaded,
                       tsp00_8ByteCounter& heapUsage,
                       tsp00_Int4&         hashSize,
                       tgg00_FileId&       versionTree,
	                     tgg00_FileId&       versionInvTree,
                       tsp00_C512&         versionDesc) {  // PTS 1117690
  tak_kernel_sink* pSink = &acv.a_kernel_sink;
  short      error;
  tsp00_Bool found;

  if (isFirstCall) {
    found = ak341LibOmsInterfacePtr->GetFirstVersion(REINTERPRET_CAST(IliveCacheSink*, pSink), versionId, createDate, createTime,
                                            openDate, openTime, consistentView, isMarked, isOpen, isUnloaded,
                                            heapUsage, hashSize, versionTree, versionInvTree, versionDesc, error);  
  }
  else {
    found = ak341LibOmsInterfacePtr->GetNextVersion(REINTERPRET_CAST(IliveCacheSink*, pSink), versionId, createDate, createTime,
                                            openDate, openTime, consistentView, isMarked, isOpen, isUnloaded,
                                            heapUsage, hashSize, versionTree, versionInvTree, versionDesc, error);  
  }

  if (error != 0){
    a07_b_put_error(acv, error, 1);   // PTS 1126695
  }

  return found;
}

/*-----------------------------------------------------------------------------------*/

// PTS 1117690
externC
tsp00_Bool ak341NextDiagnoseEntry (tak_all_command_glob& acv,
                       tsp00_Int2         &phase,
                       tsp00_Int4         &session,
                       tsp00_C24          &component,
                       tsp00_C512         &entry) 
{  
  tak_kernel_sink* pSink = &acv.a_kernel_sink;
  short      error;
  tsp00_Bool found;

  // TODO: Eventuell andere Senke/Session auswählen
  found = ak341LibOmsInterfacePtr->GetNextDiagnoseEntry(phase,
                               REINTERPRET_CAST(IliveCacheSink*, pSink), 
                               session,
                               component,
                               entry,
                               error);  

  if (error != 0){
    a07_b_put_error(acv, error, 1);   
  }

  return found;
}

/*-----------------------------------------------------------------------------------*/

//externCpp void
//ak341NextObj (tak_all_command_glob  *acv,
//            tgg91_TransNo           *ConsistentView,
//            tgg01_ContainerId       *ObjFile,
//            tgg01_OmsVersionContext *VersionContext,
//            tgg92_KernelOid         *CurrOid,
//            tsp00_Int4              ObjBodySize,
//            void*                   pObjBody,
//            tgg91_PageRef           *ResultObjVers,
//            tsp00_Int4&             ObjHistReadCount, // PTS 1107819 TA 13/09/2000
//            short                   *e)
//           
//{
//  bool       bWithObject;
//  tsp00_Int4 ObjBodyLen;
//  ObjHistReadCount = 0;
//  bWithObject = (NULL != pObjBody);
//  ak341SetConsistentView (acv, ConsistentView);
//  
//  if ((!CurrOid->gg92IsNil()) && (VERSION_OBJ_PAGE_NO_GG92 == CurrOid->gg92GetPno())) {
//    ak341VersionNextObj (*acv, VersionContext,
//      *CurrOid, 
//      ObjFile->fileDirFileId_gg00(),
//      ObjBodySize, 
//      REINTERPRET_CAST(tsp00_MoveObj*, pObjBody), 
//      *ResultObjVers, *e); 
//  }
//  else {
//    if (acv->a_transinf.tri_trans.trConsistView_gg00.gg90IsNil()) {
//      kb04StartConsistentTrans (acv->a_transinf.tri_trans, false /* isOmsVersion */ );
//      *e = acv->a_transinf.tri_trans.trError_gg00;
//      if (e_ok != *e) {
//        return;
//      }
//      *ConsistentView = acv->a_transinf.tri_trans.trConsistView_gg00; 
//    }
//    do {
//      kb04NextObjConsistent (acv->a_transinf.tri_trans,
//        *ObjFile, *CurrOid, bWithObject, ObjBodySize, pObjBody, ObjHistReadCount, *ResultObjVers);  
//      *e = acv->a_transinf.tri_trans.trError_gg00;
//      if (e_ok == *e) {
//        if (VersionContext) {
//          if (ak341UpdatedInVersion (*acv, VersionContext->ovc_tree, *ObjFile, *CurrOid, ObjBodySize, ObjBodyLen, 
//            REINTERPRET_CAST(tsp00_MoveObj*, pObjBody), *e)) {
//            if (e_object_not_found == *e) {
//              *e = e_ok;
//              continue; /* ADIS 1001698, T.A. 10.11.1998 */
//            }
//          }
//        }
//        return;
//      }
//    }
//    while (e_ok == *e);
//     
//    if ((e_no_next_object == *e) && VersionContext) // PTS 1109340
//    {
//      CurrOid->gg92SetPno(VERSION_OBJ_PAGE_NO_GG92);
//      CurrOid->gg92SetPos(0);
//      ak341VersionNextObj (*acv, VersionContext,
//        *CurrOid,
//        ObjFile->fileDirFileId_gg00(),
//        ObjBodySize, 
//        REINTERPRET_CAST(tsp00_MoveObj*, pObjBody), 
//        *ResultObjVers, *e);      
//    }
//  }
//}

/*-----------------------------------------------------------------------------------*/
// PTS 1120478
externCpp void
ak341NextOids (tak_all_command_glob    &acv,
               tgg91_TransNo           &ConsistentView,
               tgg01_ContainerId       &ObjFile,
               tgg01_OmsVersionContext *VersionContext,
               tsp00_Bool               bOnlyHdrVarObjects,
               tgg92_KernelOid         &CurrOid,
               tsp00_Int4              &noOfOid,
               tgg92_KernelOid         *pOid,
               tgg91_PageRef           *pObjVers,
               tsp00_Int4              &LogReadAccesses,
               tgg00_BasisError        &e)
{
  ak341SetConsistentView (&acv, &ConsistentView);
  
  tsp00_Int4 maxNoOfOid = noOfOid;
  noOfOid = 0;
  e = e_ok;
  if (CurrOid.gg92IsNil() || CurrOid.gg92GetPno() != VERSION_OBJ_PAGE_NO_GG92){
    // Read next kernel object
    if (acv.a_transinf.tri_trans.trConsistView_gg00.gg90IsNil()) {
      kb04StartConsistentTrans (acv.a_transinf.tri_trans, false /* isOmsVersion */ );
      e = acv.a_transinf.tri_trans.trError_gg00;
      if (e_ok != e) {
        return;
      }
      ConsistentView = acv.a_transinf.tri_trans.trConsistView_gg00; 
    }

    acv.a_transinf.tri_trans.trError_gg00 = e_ok;
    LogReadAccesses = 0;
    do {
      noOfOid = maxNoOfOid; 
      kb04NextOidsConsistent (acv.a_transinf.tri_trans, ObjFile, (bOnlyHdrVarObjects!=0),  CurrOid, 
                              noOfOid, pOid, pObjVers, LogReadAccesses);  
      e = acv.a_transinf.tri_trans.trError_gg00;
      if (e_ok == e || (e_no_next_object == e && noOfOid > 0)) {
        if (VersionContext) {
          // Remember last read oid in case, that the following filtering removes all
          // oids and therefore a new call of kb04NextOidsConsistent is needed
          CurrOid = pOid[noOfOid-1];

          int i,j;
          for (i=0,j=0; i<noOfOid; ++i){
            // Test whether object has been modified in an unloaded version. If so
            // the object is either changed or marked as delete:
            // 1.) If it is changed, then the oid should be returned by the method. If 
            //     the object itself is accessed later, then the correct version of the 
            //     object is read.
            // 2.) If it is marked as deleted, then oid should be skipped. 
            tsp00_Int4 ObjBodyLen = 0; // Unused parameter
            if (ak341UpdatedInVersion (acv, VersionContext->ovc_tree, ObjFile, pOid[i],  
                                       0, ObjBodyLen, NULL, e, false)) {
              if (e_object_not_found == e) {  // PTS 1125814
                // If object is marked as deleted, then the error code e_object_not_found
                // is returned. As deleted object should be ignored, reset the error-code.
                e = e_ok;
                continue;
              }
            }

            if (i != j){
              // Compress Oid-array if possible
              pOid[j] = pOid[i];
              pObjVers[j] = pObjVers[i];
            }
            ++j;
          }
          noOfOid = j;
        }
      }
    } while (noOfOid == 0 && e_ok == e);
  }

  if (e != e_ok && e != e_no_next_object){
    // An error occured
    noOfOid = 0;
    return;
  }
     
  if (noOfOid == 0 && VersionContext){ 
    noOfOid = maxNoOfOid;
    // All objects from the kernel have been read, so try to read objects of unloaded version 
    if (VERSION_OBJ_PAGE_NO_GG92 != CurrOid.gg92GetPno()){
      // Set oid to read first object from version
      CurrOid.gg92SetPno(VERSION_OBJ_PAGE_NO_GG92);
      CurrOid.gg92SetPos(0);
      CurrOid.gg92SetFrameVers(0);
    }
    // Read next oid from version
    ak341VersionNextOids (acv, VersionContext, 
      ObjFile.fileDirFileId_gg00(),
      CurrOid, 
      noOfOid, pOid, e);      
  }

  if (e != e_ok && e != e_no_next_object){
    // An error occured
    noOfOid = 0;
    return;
  }
}

/*-----------------------------------------------------------------------------------*/

externC void
ak341MonitorHeapContent(tak_all_command_glob& acv,
                        tak40_show_glob& a41v)
{
    ROUTINE_DBG_MEO00 ("ak341MonitorHeapContent");

    const pasbool c_null     = true;
    const pasbool c_to_undef = true;

    for (LVCMem_Allocator::ContentIterator Iter = LVCMem_Allocator::LVCAllocatorInstance().Begin();
        Iter; ++Iter)
    {
        SAPDB_UInt4                    ChunkCount;
        SAPDB_UInt4                    Size;
        int                            CallStackId;
        tsp00_Buf                      CallStack;
        char*                          pCallStack = (char*) &CallStack[0];
        SAPDB_MemFillNoCheck (pCallStack, 0, sizeof(CallStack));
        LVCMem_Allocator::SymbolVector Symbols(LVCMem_Allocator::LVCAllocatorInstance());
        Iter.Current(ChunkCount, Size, CallStackId, Symbols);
        a40move_i4    (acv, a41v, ChunkCount, !c_null, 0, !c_to_undef);
        a40move_i4    (acv, a41v, Size, !c_null, 0, !c_to_undef);
        a40move_i4    (acv, a41v, CallStackId, !c_null, 0, !c_to_undef);
        /* create long column containing call stack */
        tgg00_Rec Rec;
        Rec.recLen_gg00()  = cgg_rec_key_offset + sizeof(a41v.a4LongKey); 
        int  symbCount     = 0;
        char format[12];
        char formatWithOffset[20];
#if defined(BIT64)
        strcpy(format, "0x%016lx %s");
#else
        strcpy(format, "0x%08lx %s");
#endif
        strcpy(formatWithOffset, format);
        strcat(formatWithOffset, " + 0x%x");
        for (LVCMem_Allocator::SymbolVector::Iterator iter = Symbols.Begin(); iter != Symbols.End(); ++iter)
        {
            char* pCurr = (char*) &Rec.recBuf_gg00()[Rec.recLen_gg00()];
            if (iter->m_offset < 0)
            {
                sp77sprintf (pCurr, (sizeof(Rec) - Rec.recLen_gg00()), 
                    format, (SAPDB_Long)((char *)iter->m_pcAddr - (char *)0), iter->m_pSymb);
            }
            else
            {
                sp77sprintf (pCurr, sizeof(Rec) - Rec.recLen_gg00(), 
                    formatWithOffset, (SAPDB_Long)((char *)iter->m_pcAddr - (char *)0), iter->m_pSymb, iter->m_offset);
            }
            int moveLen        = (int) strlen (pCurr);
            Rec.recLen_gg00() += moveLen; 
            Rec.recBuf_gg00()[Rec.recLen_gg00()  ] = 13;
            Rec.recBuf_gg00()[Rec.recLen_gg00()+1] = 10;
            Rec.recLen_gg00() += 2;
            ++symbCount;
            if (symbCount <= 10)
            {
                a40move_const (acv, a41v, REINTERPRET_CAST(tsp00_MoveObjPtr, pCurr), moveLen);
            }
        }
        for (int ix = symbCount; ix < 10; ++ix)
        {
            a40move (acv, a41v, NULL, 0);
        }
        SAPDB_Byte* pBuf    = &Rec.recBuf_gg00()[cgg_rec_key_offset + sizeof(a41v.a4LongKey)]; 
        int         moveLen = Rec.recLen_gg00() - cgg_rec_key_offset - sizeof(a41v.a4LongKey);
        if (g01unicode)
        {
            if (Rec.recLen_gg00() * 2 > sizeof(Rec.recBuf_gg00()))
            {
                Rec.recLen_gg00() = sizeof(Rec.recBuf_gg00()) / 2;
            }
            // translate to kernel unicode
            SAPDB_Byte* pSource = pBuf + moveLen - 1;
            SAPDB_Byte* pDest   = pBuf + 2 * moveLen - 1; 
            for (int ix = 0; ix < moveLen; ++ix)
            {
                *pDest-- = *pSource--;
                *pDest-- = 0;
            }
            moveLen *= 2;
        }
        SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
        SysView_PseudoLongHandler::LongIdentificator longId;
        context.GetPseudoLongHandler().InsertLong(context, pBuf, moveLen, longId);
        tgg00_ShowKind showKind;
        showKind = a41v.a4sh_kind;
        a41v.a4sh_kind.becomes(sh_pseudo_long);
        a40long_descriptor(acv, a41v, *REINTERPRET_CAST(tgg00_Surrogate*, &longId), longId.m_rowCount);
        a41v.a4sh_kind = showKind;
    }
}

/*-----------------------------------------------------------------------------------*/

externC void
ak341NilConsistentTrans (tgg00_TransContext&Trans)
{
  Trans.trConsistView_gg00.gg90SetNil();
}

/*-----------------------------------------------------------------------------------*/

//externCpp void
//ak341OpenVersion (tak_all_command_glob& acv,
//                  tgg91_TransNo&        ConsistentView,
//                  tgg00_BasisError&     e) {
//  
//  e = e_ok;
//  // ++++ CHECK IF THE TRANSACTION CHANGED ANYTHING ++++
//  // IF YES THEN e = e_transaction_end_required;
//  // PTS 1111821 UH 2001-11-20
//  // k50my_state(ObjLogExists) is removed and must be exchanged with an equivalent check from the oms-layer
//}

/*-----------------------------------------------------------------------------------*/


bool ak341SetOmsTraceLevel(const char* lvl, pasbool enable) 
{
    return (ak341LibOmsInterfacePtr) ? ak341LibOmsInterfacePtr->SetTraceLevel(lvl, enable != 0) : false;
}

/*-----------------------------------------------------------------------------------*/

/* PTS 1115134 */
 
void
ak341ResetVersion (tak_all_command_glob&    acv,
                   tgg01_OmsVersionContext&  VersionContext,
                   tgg00_BasisError&         e)
{
  b01treset_file (acv.a_transinf.tri_trans, VersionContext.ovc_tree);
  e = acv.a_transinf.tri_trans.trError_gg00;
  if (e_ok == e) {
    b01treset_file (acv.a_transinf.tri_trans, VersionContext.ovc_index_tree);
    e = acv.a_transinf.tri_trans.trError_gg00;
  }
}

/*-----------------------------------------------------------------------------------*/

void
ak341SetTransactionComment (tak_all_command_glob& acv,
                            tsp00_Int2            commentLength,
                            const tak_WydeArray*  pComment)
{
  typedef union
  {
    tsp00_Int2 int_val;
    tsp00_C2   c_val;
  } t_kernelchar;

  int   ix;

  t_kernelchar* pDest;

  SAPDBMem_IRawAllocator *alloc = reinterpret_cast<SAPDBMem_IRawAllocator*>(
            acv.a_transinf.tri_trans.trAllocator_gg00);
  pDest = reinterpret_cast<t_kernelchar*>(alloc->Allocate(commentLength * 2));

  if (g01unicode)
  {
      bool  isUnicodeInstance = ((pasbool) 0 != a34IsUnicodeLivecacheUser(acv));
      if ( g01code.kernel_swap == sw_normal )
      {
          // translate to kernel unicode
          const tak_WydeArray* pString = pComment;
          const t_kernelchar *pSrcChar = reinterpret_cast<const t_kernelchar*>(pComment);
          t_kernelchar       *pDestChar = pDest;

          for (ix = 0; ix < commentLength; ++ix, ++pSrcChar, ++pDestChar)
          {
              /* if ascii client, check if characters are valid for client */
              if ((!isUnicodeInstance) && (pSrcChar->int_val > 255))
              {
                  acv.a_transinf.tri_trans.trError_gg00 = e_not_translatable;
                  return;
              }
              pDestChar->int_val = pSrcChar->int_val;
          }
      }
      else
      {
          // translate to kernel unicode
          const tak_WydeArray* pString = pComment;
          const t_kernelchar *pSrcChar = reinterpret_cast<const t_kernelchar*>(pComment);
          t_kernelchar       *pDestChar = pDest;

          for (ix = 0; ix < commentLength; ++ix, ++pSrcChar, ++pDestChar)
          {
              /* if ascii client, check if characters are valid for client */
              if ((!isUnicodeInstance) && (pSrcChar->int_val > 255))
              {
                  acv.a_transinf.tri_trans.trError_gg00 = e_not_translatable;
                  return;
              }
              pDestChar->c_val[0] = pSrcChar->c_val[1];
              pDestChar->c_val[1] = pSrcChar->c_val[0];
          }
      }
      commentLength = commentLength * 2;
  }
  else
  {
      tsp00_Char           *pchar = reinterpret_cast<SAPDB_Char*>(pDest);
      const t_kernelchar   *pkernelchar = reinterpret_cast<const t_kernelchar*>(pComment);

      for (int ix = 0; ix < commentLength; ++ix, ++pkernelchar, ++pchar)
      {
          if ( pkernelchar->int_val > 255 )
          {
              acv.a_transinf.tri_trans.trError_gg00 = e_not_translatable;
              return;
          }
          if ( g01code.kernel_swap == sw_normal )
              *pchar = pkernelchar->c_val[1];
          else
              *pchar = pkernelchar->c_val[0];
      }
  }
  kb04SetTransactionComment (acv.a_transinf.tri_trans, 
    commentLength, 
    reinterpret_cast<SAPDB_Char*>(pDest));

  alloc->Deallocate(pDest);
}; 

/*-----------------------------------------------------------------------------------*/


externC
void ak341Shutdown()
{
  TheLiveCacheAllocator.DisableClientInquiry();
}

/*-----------------------------------------------------------------------------------*/

externC void
ak341StartConsistentTrans (tgg00_TransContext &Trans,
						   bool                isOmsVersion)  /* PTS 1130354 */
{
  Trans.trError_gg00 = e_ok;
  kb04StartConsistentTrans (Trans, isOmsVersion);
}

/*-----------------------------------------------------------------------------------*/

// externC void
// ak341UnlockObj (tgg00_TransContext	&Trans,
//                 tgg00_FileId		    &ObjFile,	
//                 tgg92_KernelOid       Oid)
     
// {
//  Trans.trError_gg00 = e_ok;
//  kb04UnlockObj (Trans, ObjFile, Oid);
// }

/*-----------------------------------------------------------------------------------*/

externCpp void
ak341UserAllocatorCtl (int msg, void  *param)
{
  ak341LibOmsInterfacePtr->UserAllocatorCtl(msg, param);
}

/*-----------------------------------------------------------------------------------*/

externC void 
ak341VarObjGuid (tsp00_C16& guid) 
{
    const GUID g = { 0x8fb0963d, 0xe5aa, 0x11d1, { 0xa9, 0x5c, 0x0, 0xa0, 0xc9, 0x43, 0x7, 0x31 } };

    SAPDB_MemCopyNoCheck (&guid, &g, sizeof(GUID));
}

/*-----------------------------------------------------------------------------------*/

bool ak341QualificationTest (void* QualFunc,
                   void*          QualValues,
                   unsigned char* Obj)
{
    class MyObj {
    public :
        bool f (void*) { return true; }
    };
    union {
        bool (MyObj::* FuncPtr) (void*);
        void* voidptr;
    } aux;
    aux.voidptr = QualFunc;
    Obj -= sizeof(void*);
    return (((MyObj*) Obj->*aux.FuncPtr)(QualValues));
}


/*-----------------------------------------------------------------------------------*/

void ak341OpMsg (bool        isOpError, 
                 SAPDB_UTF8* pMsg,
                 int         msgLen)
{
  tsp00_C40 c40;
  unsigned char* pC40    = &c40[0];
  int            restLen = sizeof(c40) ;
  SAPDB_MemFillNoCheck (c40, ' ', sizeof(c40));
  while (true)
  {
    int characterSize = (int) SAPDB_UTF8Basis::CharacterSize(pMsg);
    
    if ((0 == characterSize) || (restLen < characterSize) || (msgLen <= 0))
    {
      g01optextmsg (sp3p_console, (isOpError) ? sp3m_error : sp3m_info, csp3_ak_msg, csp3_n_obj, c40);
      if (msgLen <= 0)
      {
        return;
      }
      SAPDB_MemFillNoCheck (c40, ' ', sizeof(c40));
      pC40    = &c40[0];
      restLen = sizeof(c40);
      if (0 == characterSize)
      {
        // we are out of step
        sp77sprintf ( c40, sizeof(c40), "invalid UTF8 string, truncated");
        g01optextmsg (sp3p_console, sp3m_error, csp3_ak_msg, csp3_n_obj, c40);
        return;
      }
    }
    for (int ix = 0; ix < characterSize; ++ix)
    {
      *pC40 = *pMsg;
      ++pC40;
      ++pMsg;
    }
    restLen -= characterSize;
    msgLen  -= characterSize;
  }
}

/*-----------------------------------------------------------------------------------*/
extern "C" void b120FlushTrace( tsp00_TaskId TaskId );
#include "SAPDB/KernelCommon/Kernel_TraceTopics.hpp"  // Kernel_TraceSwitch

void ak341MultiPurpose (tak_all_command_glob* acv,
            tgg00_MessType  MessType,
            tgg00_MessType2 MessType2,
            tsp00_Int4*     BufLen,
            void*           Buf,
            tsp00_Int2*     e)
{
  *e = e_ok;
  switch (MessType) {
  case m_change :
    if (e_rollback_wanted == *REINTERPRET_CAST(tsp00_Int2*, Buf))
    {
      acv->a_rollback_info.addElement(trans_rollback);
      *REINTERPRET_CAST(tsp00_Int2*, Buf) = e_work_rolled_back;
    }
    *e      = a34ReturnCode (*REINTERPRET_CAST(tsp00_Int2*, Buf));
    *BufLen = 0;
    if (g01vtrace.vtrRetcodeCheck_gg00)
    {
      if (g01vtrace.vtrStopRetcode_gg00 == *e)
      {
        Trace_CommonEntry entry(  tgg00_Debug::fromConst(ak_sem),
                                 tgg00_VtraceType::fromConst(ak_returncode), (tsp00_Byte*) e, sizeof(*e) );
        a37stop_all_vtrace(*acv);
      }
    }
    break;
  case m_connect :
      {
          tsp1_packet* initInPacket    = acv->a_in_packet;
          tsp1_packet* initOutPacket   = acv->a_out_packet;
          tsp1_segment_ptr initRetSegm = acv->a_return_segm;
          tsp1_packet* pPacket   = REINTERPRET_CAST(tsp1_packet*, Buf);
          tsp00_Int4* packetSize = REINTERPRET_CAST(tsp00_Int4*, Buf);
          *BufLen           = *packetSize; 
          acv->a_in_packet  = pPacket;
          acv->a_out_packet = pPacket;
          PIn_RequestPacket packet(pPacket, *packetSize, false);
          acv->a_return_segm = packet.AddSegment (sp1m_dbs, sp1sm_session_sqlmode).GetRawSegment();
          a51CreateConnectReturnSegment(*acv);
          *BufLen = s26packet_len (acv->a_out_packet);
          acv->a_out_packet->sp1_header.sp1h_varpart_len = *BufLen - sizeof (acv->a_out_packet->sp1_header);
          acv->a_in_packet   = initInPacket;
          acv->a_out_packet  = initOutPacket;
          acv->a_return_segm = initRetSegm;
          break;
      }
  case m_diagnose :
      if ( mm_down == MessType2 ) // PTS 1112358
      {
#if defined(OMSTST)          
          if (-4711 == *REINTERPRET_CAST (tsp00_Int4*, Buf))
          {
              ak341NormalOmsTerminate = true;
          }
          else {
              acv->a_allocatorInfo.ai_badAllocCountDown = *REINTERPRET_CAST (tsp00_Int4*, Buf);
              if (acv->a_allocatorInfo.ai_badAllocCountDown < 0)
              {
                  acv->a_allocatorInfo.ai_badAllocCountDown = -1;
              }
          }
#endif
      }
      else if ( MessType2 == mm_provoked_error){ 
#if defined(OMSTST)          
        // indicate, that the exception was provoked by the testkernel. 
        acv->a_provoked_exception = (*reinterpret_cast<unsigned char*>(Buf)==1 ? true : false);
#endif
      }
      else if ( MessType2 == mm_file){
 	      char buf[132];
        sp77sprintf(buf, sizeof(buf), "**** VTrace flushed by program because of error=%d",
                    *reinterpret_cast<short*>(Buf));
	      a34VTrace (*acv,
				          *reinterpret_cast<tsp00_MoveObj*>(&buf[0]), 
				          strlen(buf));

	      b120FlushTrace( acv->a_transinf.tri_trans.trTaskId_gg00 );	  
      }
      else if ( MessType2 == mm_write_off){
 	      char buf[132];
        sp77sprintf(buf, sizeof(buf), "**** VTrace stopped by program because of error=%d",
                    *reinterpret_cast<short*>(Buf));
	      a34VTrace (*acv,
				          *reinterpret_cast<tsp00_MoveObj*>(&buf[0]), 
				          strlen(buf));

	      b120FlushTrace( acv->a_transinf.tri_trans.trTaskId_gg00 );	  

        a37stop_all_vtrace (*acv);
        //g01vtr_init();                                    // schaltet den fasten Pascal Vtrace aus
        //const char topic_off[] = "* 0 ";
        ////Kernel_CheckSwitch (topic_off, 3);              // schaltet die C++ Checks aus
        //Kernel_TraceSwitch(topic_off, strlen(topic_off)); // schaltet den C++ Vtrace aus
      }
      else if ( MessType2 == mm_outcopy){
        // eo670_CTraceStack();
        void*          pSession;
        pasbool*       pCancelByte;
        tsp00_TaskId   taskId;
        bool           optimizedStreamCommunication;      
        REINTERPRET_CAST(IliveCacheSink*, &acv->a_kernel_sink)->GetDefaultContext(&pSession,
          &pCancelByte, optimizedStreamCommunication, taskId);
        ak341SessionDump(acv->a_transinf.tri_trans.trTaskId_gg00,
          *REINTERPRET_CAST(tsp00_Int4*, Buf), pSession);
      }
      else if ( mm_dump == MessType2 ) {  // PTS 1106505 FF
        eo38CTraceBack(*REINTERPRET_CAST(tsp00_Addr*, Buf) );
      }
      else {  // PTS 1106505 FF              
        g01vtrace.vtrBdObject_gg00 = true;
        g01vtrace.vtrOmsNew_gg00   = true;
        g01vtrace.vtrOmsGet_gg00   = true;
        g01vtrace.vtrOmsUpd_gg00   = true;
        g01vtrace.vtrOmsFree_gg00  = true;
        g01vtrace.vtrAny_gg00      = true;
      }
      *e      = 0;
      *BufLen = 0;
      break;
  case m_set : 
    {
      switch (MessType2) {
      case mm_lock_timeout :
        acv->a_transinf.tri_trans.trObjReqTimeout_gg00 = 
          *REINTERPRET_CAST (tsp00_Int2*, Buf);
        *BufLen = 0;
        break;
      default :
        *e      = e_invalid;
        *BufLen = 0;
      }
      break;
    }
  case m_lock :
    a34Lock (*acv, REINTERPRET_CAST(tsp00_MoveObj*, Buf), *BufLen, mm_lock == MessType2, *e);
    *BufLen = 0;
    break;
  case m_show :
    if (g01unicode)
    {
      ak341OpMsg (mm_log != MessType2, REINTERPRET_CAST(SAPDB_UTF8*, Buf), *BufLen);
    }
    else
    {
      a34OpMsg (mm_log != MessType2, *REINTERPRET_CAST(tsp00_MoveObj*, Buf), *BufLen);
    }
    *BufLen = 0;
    break;
  case m_unlock :
    a34UnLock (*acv, REINTERPRET_CAST(tsp00_MoveObj*, Buf), *BufLen, mm_lock == MessType2, *e);
    *BufLen = 0;
    break;
  case m_return_error :
      {
           acv->a_dbproc_call_info.dbc_conversionError = true;
           acv->a_dbproc_call_info.dbc_errorTableId    = *REINTERPRET_CAST(tsp00_Int4*, Buf);
           break;
      }
  default :
    *e = e_invalid;
    *BufLen = 0;
  }
}


/*-----------------------------------------------------------------------------------*/

externC
pasbool ak341IsMonitorOn() {
return ak341LibOmsInterfacePtr->IsMonitorOn();
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341StartStopMonitor(pasbool doStart) {
  ak341LibOmsInterfacePtr->StartStopMonitor(doStart > 0);
  TheLiveCacheAllocator.SetMonitoring(doStart > 0);
}

/*-----------------------------------------------------------------------------------*/

class tak341_VersionRecBuf {
  struct tak341_VersionRecInfo {
    // PTS 1131966
    // As the length of the containerId has been changed from 4 to 8 bytes, the
    // maximal overall length of the record become too large by one byte (8097).
    // Because of this the header-length has been changed from 8 to 7 byte (only
    // the first 4 bytes are currently used, and the position of the m_state has
    // been moved just behind the header. The header-length was 8 byte, as the
    // kernel expect the header size to be 8 byte long. So the length of this 
    // structure is reduced by one byte and the maximal length fits into the 
    // record again. 
    unsigned char   m_header[7];
    unsigned char   m_state; // PTS 1109340
    tsp00_C8        m_oidc;
    tsp00_C8        m_containerId;

  };
  enum ct_state { CN_DELETED = 1, CN_VAROBJ = 2, CN_CONTOBJ = 4 };
public :
  tak341_VersionRecBuf() {}
  tak341_VersionRecBuf(
    const tsp00_C8&  ContainerId, 
    const tsp00_C8&  Oidc,
    bool             IsVarObj, // PTS 1109340
    tsp00_Int4       BodySize, 
    tsp00_MoveObj*   Body,
    bool             IsContObj); // PTS 1124278
  
  tsp00_C8& GetContainerId() {
    return  REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_containerId;
  }
  bool IsContObject() {  // PTS 1124278
    return ((REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_state & CN_CONTOBJ) > 0);
  }
  bool IsDeleted() {
    return ((REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_state & CN_DELETED) > 0);
  }
  bool IsVarObject() {
    return ((REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_state & CN_VAROBJ) > 0);
  }
  tgg00_BasisError IsOk() {
    return (m_rec.recLen_gg00() < 0) ? m_rec.recLen_gg00() : e_ok;
  }
  void MarkContObject() {  // PTS 1124278
    REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_state |= CN_CONTOBJ;
  }
  void MarkDeleted() {
    REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_state |= CN_DELETED;
  }
  void MarkVarObject() {
    REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec)->m_state |= CN_VAROBJ;
  }
  tsp00_Int4 ObjBodyLen() {
    return  m_rec.recLen_gg00() - OBJ_OFFSET;
  }
  void BuildKeyRec(
    const tsp00_C8& ContainerId,
    tsp00_MoveObj* pKey, int KeyLen, const tsp00_C8 &Oidc, tgg00_BasisError& e);

  tgg00_BasisError CopyObj(tsp00_Int4 Size, tsp00_MoveObj* copyBuf) {
    tgg00_BasisError e = e_ok;
    SAPDB_RangeMove (__FILE__, 1,
      sizeof(m_rec), Size,
      &m_rec, OBJ_OFFSET + 1, copyBuf, 1, ObjBodyLen(), e);
    return e;
  }
  tsp00_MoveObj* GetInfoPtr() {
    return REINTERPRET_CAST(tsp00_MoveObj*, &m_rec.recBody_gg00()[m_rec.recKeyLen_gg00()]);
  }

  tsp00_MoveObj* GetKeyPtr() {
    return REINTERPRET_CAST(tsp00_MoveObj*, &m_rec.recBody_gg00()[0]);
  }

  tgg92_KernelOid GetOid() {
    SAPDB_UInt8 i8;
    SAPDB_MemCopyNoCheck(&i8, &m_rec.recBody_gg00(), sizeof (SAPDB_UInt8));
    tgg92_OmsTypeOid oid(i8);
    return (tgg92_KernelOid&)oid;
  }

  operator tgg00_Rec&() { 
    return m_rec;
  }
private :
  tsp00_Int4 m_align; // 4 byte alignment required on hp64, otherwise bus error !
  tgg00_Rec  m_rec;
 
  enum {OBJ_OFFSET = 7 + 1 + sizeof(tsp00_C8) + sizeof(tsp00_C8) };
};

/*-----------------------------------------------------------------------------------*/

tak341_VersionRecBuf::tak341_VersionRecBuf
(
 const tsp00_C8& ContainerId,
 const tsp00_C8& Oidc, 
 bool       IsVarObj,
 tsp00_Int4 BodySize, 
 tsp00_MoveObj* Body,
 bool       IsContObj  // PTS 1124278
)
{
  tgg00_BasisError e = e_ok;
  m_rec.recLen_gg00()          = OBJ_OFFSET + BodySize; 
  m_rec.recKeyLen_gg00()       = sizeof(Oidc);
  m_rec.recVarcolCnt_gg00()    = 0;
  m_rec.recVarcolOffset_gg00() = m_rec.recLen_gg00() -
              cgg_rec_key_offset - m_rec.recKeyLen_gg00();
  tak341_VersionRecInfo* pInfo = REINTERPRET_CAST(tak341_VersionRecInfo*, &m_rec);
  SAPDB_MemCopyNoCheck(&pInfo->m_oidc, &Oidc, sizeof(pInfo->m_oidc));
  pInfo->m_containerId = ContainerId;
  pInfo->m_state       = 0; // PTS 1109340
  if (IsVarObj)
  {
    this->MarkVarObject();
  }
  if (IsContObj){  // PTS 1124278
    this->MarkContObject();
  }
  if (BodySize > 0)
  {
	  SAPDB_RangeMove (__FILE__, 2,
		  BodySize, sizeof(m_rec),
		  Body, 1, &m_rec, OBJ_OFFSET + 1, BodySize,  m_rec.recLen_gg00());
  }
}

/*-----------------------------------------------------------------------------------*/

void tak341_VersionRecBuf::BuildKeyRec(
  const tsp00_C8   &ContainerId,
  tsp00_MoveObj    *pKey, 
  int               KeyLen, 
  const tsp00_C8   &Oidc, 
  tgg00_BasisError &e) 
{
  e                            = e_ok;
  m_rec.recKeyLen_gg00()       = (tsp00_Int4)(sizeof(ContainerId) + KeyLen);
  m_rec.recLen_gg00()          = cgg_rec_key_offset + m_rec.recKeyLen_gg00() + sizeof(Oidc);
  m_rec.recVarcolCnt_gg00()    = 0;
  m_rec.recVarcolOffset_gg00() = m_rec.recLen_gg00() -
              cgg_rec_key_offset - m_rec.recKeyLen_gg00();

  *REINTERPRET_CAST(tsp00_C8*, &m_rec.recBody_gg00()) = ContainerId;
  SAPDB_RangeMove (__FILE__, 3,
    KeyLen, sizeof(m_rec.recBody_gg00()),
    pKey, 1, &m_rec.recBody_gg00(), sizeof(ContainerId) + 1, KeyLen, e);
  SAPDB_RangeMove  (__FILE__, 4,
    sizeof(Oidc), sizeof(m_rec.recBody_gg00()),
    &Oidc, 1, &m_rec.recBody_gg00(), sizeof(ContainerId) + KeyLen + 1, sizeof(Oidc), e);
}
            
/*-----------------------------------------------------------------------------------*/

externCpp
bool ak341UpdatedInVersion (tak_all_command_glob& acv,
                        tgg00_FileId&         VersionFile,
                        tgg01_ContainerId&    ObjFile, /* PTS 1109340 */
                        tgg92_KernelOid&      Oid,
                        tsp00_Int4            ObjBodySize,
                        tsp00_Int4&           ObjBodyLen,
                        tsp00_MoveObj*        Body,
                        tgg00_BasisError&     e,
                        tsp00_Bool            bWithObject)
{

  tgg00_Lkey             rk;
  tak341_VersionRecBuf Rec;
  rk.len() = sizeof(Oid);
  SAPDB_MemCopyNoCheck(&(rk.k()[0]), &Oid, sizeof(Oid));
  b02get_record (acv.a_transinf.tri_trans, VersionFile, rk, Rec);
  e = acv.a_transinf.tri_trans.trError_gg00;
  if (e_ok == e) {
    if (Rec.IsDeleted()) {
      e = e_object_not_found;
    }
    else {
      if (oftUnknownType_egg00 == ObjFile.fileObjFileType_gg00()) /* PTS 1109340 */
      {
        ObjFile.fileDirFileId_gg00() = Rec.GetContainerId();
        if (Rec.IsVarObject())
        {
          ObjFile.fileObjFileType_gg00().becomes(oftVarLenObjFile_egg00);
        }
      }
      ObjBodyLen = Rec.ObjBodyLen();

      if (Body != NULL){
        // Copy body only if it is requested  (PTS 1134709)
        e = Rec.CopyObj(ObjBodySize, Body);
      }
    }
  }
  if (e_key_not_found == e) {
    e = e_ok;
    return false;
  }
  else {
    return true;
  }
}

/*-----------------------------------------------------------------------------------*/

externCpp
void ak341VersionDelObj(tak_all_command_glob& acv,
                        tgg00_FileId&            VersionFile,
                        const tgg01_ContainerId& ObjFile, // PTS 1109340
                        tgg92_KernelOid&         Oid,
                        bool                     DoInsert,
                        tgg00_BasisError&        e)
{
  tak341_VersionRecBuf Rec(
      ObjFile.fileDirFileId_gg00(), 
      (tsp00_C8&)Oid, 
      ObjFile.fileObjFileType_gg00() == oftVarLenObjFile_egg00, 
      0, NULL, false);
  e = Rec.IsOk();
  if (e_ok == e) {
    Rec.MarkDeleted();
    do {
      if (DoInsert) {
        b02add_record (acv.a_transinf.tri_trans, VersionFile, Rec);
      }
      else {
        b02repl_record (acv.a_transinf.tri_trans, VersionFile, Rec);
      }
      e = acv.a_transinf.tri_trans.trError_gg00;
      if ((e == e_duplicate_key) && (DoInsert)) {
        DoInsert = false;
      }
      else {
        break;
      }
    }
    while (true);
  }
}
                        
/*-----------------------------------------------------------------------------------*/                        

externCpp
void ak341VersionGetObjViaKey (tak_all_command_glob& acv,
                              tgg01_OmsVersionContext* VersionContext,
                              tsp00_Int4               KeyLen,
                              tsp00_MoveObj*           pKey,
                              tsp00_C8                 ClsId,
                              tsp00_Int4               ObjBodySize,
                              tsp00_MoveObj*           Body,
                              tgg92_KernelOid&         Oid,
                              tgg00_BasisError&        e)
{
  tgg00_Lkey lKey;
  tak341_VersionRecBuf Rec;
  
  e = e_ok;
  lKey.len() = (tsp00_Int4) sizeof(tsp00_C8) + KeyLen;
  SAPDB_MemCopyNoCheck (&(lKey.k()[0]), &ClsId, sizeof(ClsId));
  SAPDB_RangeMove (__FILE__, 5, 
    KeyLen, sizeof(lKey.k()), pKey, 1, &lKey.k(), sizeof(tsp00_C8) + 1, KeyLen, e);
  if (e_ok == e) {
    b02get_record (acv.a_transinf.tri_trans, VersionContext->ovc_index_tree, lKey, Rec);
    e = acv.a_transinf.tri_trans.trError_gg00;
  }
  if (e_ok == e) {
    tgg01_ContainerId dummyId;
    SAPDB_MemCopyNoCheck (&Oid, Rec.GetInfoPtr(), sizeof(Oid));
    if (ak341UpdatedInVersion (acv, VersionContext->ovc_tree, dummyId, Oid, ObjBodySize, ObjBodySize, Body, e)) {
      if (e_object_not_found == e) {
        e = e_hash_key_not_found;
      }
    }
    else {
      e = e_invalid;
    }
  }
  else {
    if (e_key_not_found == e) {
       e = e_hash_key_not_found;
    }
  }
}

/*-----------------------------------------------------------------------------------*/     

externC
void ak341VersionNextObj (tak_all_command_glob& acv,
                          tgg01_OmsVersionContext* VersionContext,
                          tgg92_KernelOid&         Oid,
                          tsp00_C8&                wantedContainerId,
                          tsp00_Int4               ObjBodySize,
                          tsp00_MoveObj*           Body,
                          tgg91_PageRef&           ResultObjVers,
                          tgg00_BasisError&        e)
{
  
  const pasbool c_inclusive = true;
  tgg00_Lkey           lKey;
  tak341_VersionRecBuf Rec;

  lKey.len() = sizeof(Oid);
  acv.a_transinf.tri_trans.trError_gg00 = e_ok;
  do {
    SAPDB_MemCopyNoCheck (&(lKey.k()[0]), &Oid, sizeof(Oid));
    b02next_record (acv.a_transinf.tri_trans, VersionContext->ovc_tree,
      lKey, !c_inclusive, Rec);
    e = acv.a_transinf.tri_trans.trError_gg00;
    if (e_key_not_found == e) {
      e = e_ok;
    }
    if (e_ok == e) {
      // Skip continuation-objects of var-objects (PTS 1124278)
      if (Rec.IsContObject()){
        continue;
      }
      Oid = Rec.GetOid();
      if ((wantedContainerId == Rec.GetContainerId()) && (!Rec.IsDeleted())) {
        Rec.CopyObj(ObjBodySize, Body);
        ResultObjVers.gg91SetDummyRef();
        return;
      }
    }
    else {
      if (e_no_next_record == e) {
        e = e_no_next_object;
      }
      Oid.gg92SetNil(); // PTS 1121568
      return;
    }
  }
  while (true);
}

/*-----------------------------------------------------------------------------------*/     
// PTS 1120478
externC
void ak341VersionNextOids (tak_all_command_glob    &acv,
                           tgg01_OmsVersionContext *VersionContext,
                           tsp00_C8&                wantedContainerId,
                           tgg92_KernelOid         &CurrOid,
                           tsp00_Int4              &noOfOid,
                           tgg92_KernelOid         *pOid,
                           tgg00_BasisError        &e)
{
  const pasbool c_inclusive = true;
  tgg00_Lkey           lKey;
  tak341_VersionRecBuf Rec;

  lKey.len() = sizeof(tgg92_KernelOid);
  acv.a_transinf.tri_trans.trError_gg00 = e_ok;
  int maxNoOfOid = noOfOid;
  noOfOid = 0;
  while (noOfOid < maxNoOfOid){
    SAPDB_MemCopyNoCheck (&(lKey.k()[0]), &CurrOid, sizeof(tgg92_KernelOid));
    // Read next oid
    b02next_record (acv.a_transinf.tri_trans, VersionContext->ovc_tree,
                    lKey, !c_inclusive, Rec);
    e = acv.a_transinf.tri_trans.trError_gg00;
    if (e_key_not_found == e) {
      e = e_ok;
    }

    if (e_ok == e) {
      // Remember oid for next iteration
      CurrOid = Rec.GetOid();
      if ((wantedContainerId == Rec.GetContainerId()) && (!Rec.IsDeleted())) {
        pOid[noOfOid++] = CurrOid;
      }
    }
    else {
      if (e_no_next_record == e) {
        // All records have been read
        e = e_no_next_object;
      }

      // Iterator is empty or an error occured while reading oid
      return;
    }
  }
}

/*-----------------------------------------------------------------------------------*/     

externC
void ak341VersionNextKey (tak_all_command_glob&    acv,
                          tgg01_OmsVersionContext* VersionContext,
                          const tgg01_ContainerId& ObjFile,
                          void*                    pKey,
                          tsp00_Int4               KeyLen,
                          bool                     Inclusive,
                          void*                    pUpperKey,
                          tgg92_KernelOid&         Oid,
                          tgg00_BasisError&        e)
{
  
  const pasbool c_inclusive = true;
  
  tgg00_Lkey             lKey;
  tak341_VersionRecBuf Rec;
  
  e = e_ok;
  acv.a_transinf.tri_trans.trError_gg00 = e_ok;
  
  lKey.len() = (tsp00_Int4)sizeof(tsp00_C8) + KeyLen;
  SAPDB_MemCopyNoCheck(&(lKey.k()[0]), &ObjFile.fileDirFileId_gg00(), sizeof(ObjFile.fileDirFileId_gg00()));
  SAPDB_RangeMove (__FILE__, 6, 
    KeyLen, sizeof(lKey.k()), pKey, 1, &lKey.k(), sizeof(tsp00_C8) + 1, KeyLen, e);

  b02next_record (acv.a_transinf.tri_trans, VersionContext->ovc_index_tree,
    lKey, Inclusive, Rec);
  e = acv.a_transinf.tri_trans.trError_gg00;
  if (e_key_not_found == e)
  {
    e = e_ok;
  }
  if (e_ok == e)
  {
    if (*REINTERPRET_CAST(tsp00_C8*, Rec.GetKeyPtr()) != ObjFile.fileDirFileId_gg00())
    {
      e = e_no_next_object;
    }
    else
    {
      if (memcmp(&(Rec.GetKeyPtr()[sizeof(ObjFile.fileDirFileId_gg00())]), pUpperKey, KeyLen) > 0)
      {
        e = e_no_next_object;
      }
    }
    SAPDB_MemCopyNoCheck (&Oid, Rec.GetInfoPtr(), sizeof(Oid));
    SAPDB_MemCopyNoCheck (pKey, &(Rec.GetKeyPtr()[sizeof(ObjFile.fileDirFileId_gg00())]), KeyLen);
  }
  else 
  {
    if (e_no_next_record == e) 
    {
      SAPDB_MemFillNoCheck(pKey, 0xff, KeyLen);   // PTS 1119480
      e = e_no_next_object;
    }
  }
}

/*-----------------------------------------------------------------------------------*/     
// PTS 1119480
externC
void ak341VersionPrevKey (tak_all_command_glob&    acv,
                          tgg01_OmsVersionContext* VersionContext,
                          const tgg01_ContainerId& ObjFile,
                          void*                    pKey,
                          tsp00_Int4               KeyLen,
                          bool                     Inclusive,
                          void*                    pLowerKey,
                          tgg92_KernelOid&         Oid,
                          tgg00_BasisError&        e)
{
  
  const pasbool c_inclusive = true;
  
  tgg00_Lkey           lKey;
  tak341_VersionRecBuf Rec;
  
  e = e_ok;
  acv.a_transinf.tri_trans.trError_gg00 = e_ok;
  
  lKey.len() = (tsp00_Int4)sizeof(tsp00_C8) + KeyLen;
  SAPDB_MemCopyNoCheck(&(lKey.k()[0]), &ObjFile.fileDirFileId_gg00(), sizeof(ObjFile.fileDirFileId_gg00()));
  SAPDB_RangeMove (__FILE__, 8, KeyLen, sizeof(lKey.k()), pKey, 1, &lKey.k(), 
         sizeof(tsp00_C8) + 1, KeyLen, e);
  
  b02prev_record (acv.a_transinf.tri_trans, VersionContext->ovc_index_tree,
                  lKey, Inclusive, Rec);
  e = acv.a_transinf.tri_trans.trError_gg00;
  if (e_key_not_found == e) {
    e = e_ok;
  }
  if (e_ok == e) {
    if (*REINTERPRET_CAST(tsp00_C8*, Rec.GetKeyPtr()) != ObjFile.fileDirFileId_gg00()){
      e = e_no_next_object;
    }
    else {
      if (memcmp(&(Rec.GetKeyPtr()[sizeof(ObjFile.fileDirFileId_gg00())]), pLowerKey, KeyLen) < 0) {
        e = e_no_next_object;
      }
    }
    SAPDB_MemCopyNoCheck (&Oid, Rec.GetInfoPtr(), sizeof(Oid));
    SAPDB_MemCopyNoCheck (pKey, &(Rec.GetKeyPtr()[sizeof(ObjFile.fileDirFileId_gg00())]), KeyLen);
  }
  else {
    if (e_no_prev_record == e) {
      SAPDB_MemFillNoCheck(pKey, 0x0, KeyLen);   // PTS 1119480
      e = e_no_next_object;
    }
  }
}

/*-----------------------------------------------------------------------------------*/ 

externCpp
void ak341VersionUpdObj(tak_all_command_glob& acv,
                        tgg01_OmsVersionContext* VersionContext,
                        const tgg01_ContainerId& ObjFile, // PTS 1109340
                        tgg92_KernelOid&         Oid,
                        bool                     DoInsert,
                        tsp00_Int4               BodySize,
                        tsp00_MoveObj*           Body,
                        //tgg00_ObjColDesc&      ColDesc,    // PTS !"§$
                        tsp00_Int4               KeyPos,     // PTS !"§$
                        tsp00_Int4               KeyLen,     // PTS !"§$
                        bool                     contObj,    // PTS 1124278
                        tgg00_BasisError&        e)
{
  tak341_VersionRecBuf Rec(
    ObjFile.fileDirFileId_gg00(), 
    (tsp00_C8&)Oid, 
    ObjFile.fileObjFileType_gg00() == oftVarLenObjFile_egg00, 
    BodySize, Body, contObj);
  e = Rec.IsOk();
  if (e_ok == e) {
    do {
      if (DoInsert) {
        b02add_record (acv.a_transinf.tri_trans, VersionContext->ovc_tree, Rec);
        if ((VERSION_OBJ_PAGE_NO_GG92 == Oid.gg92GetPno()) && (e_ok == acv.a_transinf.tri_trans.trError_gg00)) {
          //if (ColDesc.colHashCount_gg00 > 0) {
          if (KeyLen > 0){
            //tgg00_StackEntry& st = *REINTERPRET_CAST(tgg00_StackEntry*, ColDesc.colHashList_gg00);
            //Rec.BuildKeyRec(ObjFile.fileObjFileNo_gg00(), &Body[st.epos()-1], st.elen_var(), Oid, e);
            Rec.BuildKeyRec(ObjFile.fileDirFileId_gg00(), &Body[KeyPos], KeyLen, (tsp00_C8&)Oid, e);
            if (e_ok == e) {
              b02add_record (acv.a_transinf.tri_trans, VersionContext->ovc_index_tree, Rec);
              if (e_duplicate_key == acv.a_transinf.tri_trans.trError_gg00)
              {
                b02repl_record (acv.a_transinf.tri_trans, VersionContext->ovc_index_tree, Rec);
              }
            }
          }
        }
      }
      else {
        b02repl_record (acv.a_transinf.tri_trans, VersionContext->ovc_tree, Rec);
      }
      e = acv.a_transinf.tri_trans.trError_gg00;
      if ((e == e_duplicate_key) && (DoInsert)) {
        DoInsert = false;
      }
      else {
        break;
      }
    }
    while (true);
  }
}
	  
/*-----------------------------------------------------------------------------------*/
/*                               Implementation of Dump Manager                      */
/*-----------------------------------------------------------------------------------*/

void* Dump_cak341::GetOmsSession()
{
  return m_session;
}

/*-----------------------------------------------------------------------------------*/
     
void Dump_cak341::SetDumpLabel (const char* label, int dumpType)
{
  this->Flush();
  SAPDB_MemFillNoCheck(&m_label, ' ', sizeof(m_label));
  int length = (int)strlen(label);
  if (length > sizeof(m_label))
  {
    length = sizeof(m_label);
  }
  SAPDB_MemCopyNoCheck(&m_label, label, length);
  m_dumpType = dumpType;
}

/*-----------------------------------------------------------------------------------*/

void Dump_cak341::Dump (void* p, int len)
{
  if (m_pos + len > mxsp_buf)
  {
    this->Flush();
  }
  SAPDB_MemCopyNoCheck(&m_buf[m_pos-1], p, len);
  m_pos += len;
}

/*-----------------------------------------------------------------------------------*/

bool Dump_cak341::Synchronize() const
{
  return m_sync;
}

/*-----------------------------------------------------------------------------------*/

void Dump_cak341::TriggerDump()
{
  if (vf_ok == m_vfReturn)
  {
    ak341LibOmsInterfacePtr->Dump(*this);
  }
}

/*-----------------------------------------------------------------------------------*/

void Dump_cak341::Flush()
{
#define c_dumpHeaderLength 12
  if (m_headerPos > 0)
  {
    SAPDB_MemCopyNoCheck(&m_buf[m_headerPos], &m_label[0], sizeof(m_label));
    m_headerPos += sizeof(m_label);
    SAPDB_MemCopyNoCheck(&m_buf[m_headerPos], &m_dumpType, sizeof(m_dumpType));
    m_headerPos += sizeof(m_dumpType);
    tsp00_Int2 len = m_pos - 1;
    SAPDB_MemCopyNoCheck(&m_buf[m_headerPos], &len, sizeof(len));
  }
  g01new_dump_page (m_file, m_buf, m_pno, m_pos, m_vfReturn, m_errTxt);
  m_headerPos = m_pos - 1;
  m_pos      += c_dumpHeaderLength;
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341Dump(tgg00_VfFileref& f,
          tsp00_Page&       buf, 
          tsp00_Int4&     pno, 
          int&            pos,
          tsp00_VfReturn&  vfReturn,
          tsp00_ErrText&    errTxt,
          pasbool         enterRegions)
{
  Dump_cak341 dumpObj(NULL, f, buf, pno, pos, vfReturn, errTxt, enterRegions != 0);
  dumpObj.TriggerDump();
}

/*-----------------------------------------------------------------------------------*/

void ak341SessionDump(tsp00_TaskId taskId, tsp00_Int4 errorNo, void* session)
{
  bool            closeFile = false;
  tgg00_VfFileref  fileRef;
  tsp00_Buf       buf;
  int             pos;
  tsp00_Int4      pno;
  tsp00_VfReturn   vfReturn;
  tsp00_ErrText     errTxt;
  tsp00_VFilename dumpFn;
  char*           pDumpFn = REINTERPRET_CAST(char*, &dumpFn[0]); 
  fileRef.buf_cnt = 0;
  SAPDB_MemFillNoCheck(pDumpFn, ' ', sizeof(dumpFn));
  if (errorNo != 0)
  {
    sprintf(pDumpFn, "OMS_T%d_%d.dmp", taskId, errorNo);
  }
  else
  {
    sprintf(pDumpFn, "OMS_T.dmp", taskId);
  }
  pDumpFn[strlen(pDumpFn)] = ' ';
  vfopen (dumpFn, &fileRef.no, vfReturn, errTxt);
  if (vf_ok == vfReturn)
  {
    closeFile = true;
    Dump_cak341 dumpObj(session, fileRef, buf, pno, pos, vfReturn, errTxt, true);
    dumpObj.TriggerDump();
  }
  if (closeFile)
  {
    vfclose(fileRef.no, vfReturn, errTxt);
  }
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341SetDumpError(tsp00_Int4 errorNo, int action)
{
    OMS_DiagnoseAction omsAction;
    switch (action)
    {
    case cak_i_to :
        omsAction = OMS_DIAGNOSE_DUMP;
        break;
    case cak_i_stop :
        omsAction = OMS_DIAGNOSE_STOP;
        break;
    case cak_i_flush :
        omsAction = OMS_DIAGNOSE_FLUSH;
        break;
    }
    ak341LibOmsInterfacePtr->SetDumpError(errorNo, omsAction);
}

/*-----------------------------------------------------------------------------------*/
/*                               Implementation of Tracer                            */
/*-----------------------------------------------------------------------------------*/

Trace_cak341::Trace_cak341(const char* fn) 
: m_file(-1)
{
  tsp00_VfReturn_Param ret;
  if (0 < strlen(fn))
  {
    m_file = eo15OpenMessageFile(fn, 0, false, ret);
  }
}

/*-----------------------------------------------------------------------------------*/

void Trace_cak341::Trace( const char*fmt, ... )
{
  va_list args;
  va_start (args, fmt);
  
  char buffer[132];
  sp77vsprintf ( buffer, sizeof(buffer), fmt, args );
  if (-1 != m_file)
  {
    tsp00_VfReturn_Param ret;
    strcat(buffer, "\n");
    eo15WriteMessageFile(m_file, buffer, ret);
    eo15SyncMessageFile( m_file, ret);
  }
  else
  {
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, vGetAcvPtrFromCurrentTask());

    Trace_CommonEntry entry( tgg00_Debug::fromConst(ta),
        tgg00_VtraceType::fromConst(td_none), REINTERPRET_CAST( tsp00_BytePtr, buffer ), 
        (SAPDB_Int4) strlen(buffer) );
  }
  va_end (args);
}

/*-----------------------------------------------------------------------------------*/

externC
void ak341Trace(const char* buffer)
{
  if (ak341pTrace)
  {
    ak341pTrace->Trace("%s", buffer);
  }
}

