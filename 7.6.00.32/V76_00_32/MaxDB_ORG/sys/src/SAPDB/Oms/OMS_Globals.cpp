/*!
 * \file    OMS_Globals.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Global methods and variables.
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

#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_DummyKernelCallback.hpp"
#include "Oms/OMS_KernelDefaultInterface.hpp"
#include "Oms/OMS_DumpInterface.hpp"
#include "Oms/OMS_VersionDictionary.hpp"
#include "Oms/OMS_MonitorDirectory.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_AFX.h"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"


/*----------------------------------------------------------------------*/

static OMS_DummyKernelCallback DummySink;

IliveCacheSink *OMS_GetProxySinkAddr() 
{
  return &DummySink; 
}

/*----------------------------------------------------------------------*/

extern "C" void co10_initSingletons()
{
  // initialize all singletons to sensible values - on load in xregcomp
  OMS_Globals::InitSingletons();
}

/*----------------------------------------------------------------------*/

extern "C" void* co10_GetInterface() 
{
  if (OMS_Globals::m_libOmsInterfaceInstance == NULL)
    OMS_Globals::InitSingletons();
  return reinterpret_cast<void*>(OMS_Globals::m_libOmsInterfaceInstance);
}

/*----------------------------------------------------------------------*/

extern void omsGetLiveCacheVersion(char* pBuf, int bufSize)
{
  if (NULL == OMS_Globals::KernelInterfaceInstance->GetKnlVersion(pBuf, bufSize))
  {
    OMS_Globals::Throw(e_buffer_too_small, "omsGetLiveCacheVersion", __MY_FILE__, __LINE__);
  }
}

/*----------------------------------------------------------------------*/

extern unsigned int omsGetLCVersion()
{
  return <MAJOR_VERSION><MINOR_VERSION><CORRECTION_LEVEL><FORMATED3_BUILD_PREFIX>;
}

/*----------------------------------------------------------------------*/

extern void omsSimConsoleMessage(const char* msg)
{
  if (OMS_Globals::KernelInterfaceInstance) OMS_Globals::KernelInterfaceInstance->ConsoleMessage(msg);
}

/*----------------------------------------------------------------------*/

extern void AFX_EXT_CLASS omsResetForSimulator()
{
  OMS_Globals::m_globalsInstance->ResetInfo();
}

/*----------------------------------------------------------------------*/

OMS_Globals			            *OMS_Globals::m_globalsInstance = NULL;
OMS_GetSinkPtrFunc		      OMS_Globals::GetSinkAddrFunc = OMS_GetProxySinkAddr;
LVC_KernelInterface*		    OMS_Globals::KernelInterfaceInstance = NULL;
OMS_LibOmsInterfaceInstance	*OMS_Globals::m_libOmsInterfaceInstance = NULL;

static RTESync_Spinlock omsSyncDumpError;

static double			omsAllocatorWrapperInstanceSpace[sizeof(OMS_GlobalAllocatorWrapper) / sizeof(double) + 1];
static double			userAllocatorWrapperInstanceSpace[sizeof(OMS_GlobalAllocatorWrapper) / sizeof(double) + 1];
static double			SQLDBCAllocatorWrapperInstanceSpace[sizeof(OMS_GlobalAllocatorWrapperThrowsNothing) / sizeof(double) + 1];
static double			omsGlobalsInstanceSpace[sizeof(OMS_Globals) / sizeof(double) + 1];
static double			omsVersionDictInstanceSpace[sizeof(OMS_VersionDictionary) / sizeof(double) + 1];
static double			omsMonitorDirInstanceSpace[sizeof(OMS_MonitorDirectory) / sizeof(double) + 1];
static double			omsUserAllocatorDirInstanceSpace[sizeof(OMS_GlobalUserAllocatorDirectory) / sizeof(double) + 1];
static double			omsAnchorDirInstanceSpace[sizeof(OMS_GlobalAnchorDirectory) / sizeof(double) + 1];
static void       *omsLibOmsInterfaceInstanceSpace = NULL;

/*----------------------------------------------------------------------*/
/* PTS 1115231 */
externCpp
bool omsIsUnicodeInstance()
{
    IliveCacheSink* pSink = OMS_Globals::GetCurrentLcSink();
    bool  isUnicodeInstance = false;
    if (pSink)
    {
        short error;
        pSink->IsUnicodeInstance(isUnicodeInstance, error);
        if (0 != error)
        {
            OMS_Globals::Throw(error, "omsIsUnicodeInstance", __MY_FILE__, __LINE__);
        }
    }
    return isUnicodeInstance;
}

/*----------------------------------------------------------------------*/
/* PTS 1135287 */
externCpp
bool omsIsOneDBSystem()
{
    short error;
    bool isOneDBSystem = OMS_Globals::GetKernelInterface()->IsOneDBSystem(error);
    if (0 != error)
    {
        OMS_Globals::Throw(error, "isOneDBSystem", __MY_FILE__, __LINE__);
    }
    return isOneDBSystem;
}

/*----------------------------------------------------------------------*/
OMS_Globals::OMS_Globals()
  : m_omsAllocatorWrapper(*reinterpret_cast<OMS_GlobalAllocatorWrapper*>(omsAllocatorWrapperInstanceSpace))
  , m_userAllocatorWrapper(*reinterpret_cast<OMS_GlobalAllocatorWrapper*>(userAllocatorWrapperInstanceSpace))
  , m_SQLDBCAllocatorWrapper(*reinterpret_cast<OMS_GlobalAllocatorWrapperThrowsNothing*>(SQLDBCAllocatorWrapperInstanceSpace))
  , m_versionDictionary(*reinterpret_cast<OMS_VersionDictionary*>(omsVersionDictInstanceSpace))
  , m_monitorDirectory(*reinterpret_cast<OMS_MonitorDirectory*>(omsMonitorDirInstanceSpace))
  , m_globalUserAllocatorDirectory(*reinterpret_cast<OMS_GlobalUserAllocatorDirectory*>(omsUserAllocatorDirInstanceSpace))
  , m_globalAnchorDirectory(*reinterpret_cast<OMS_GlobalAnchorDirectory*>(omsAnchorDirInstanceSpace))
  , m_versionIter() 
  , m_host(HostUnknown)
  #if defined(OMSTST)
  , m_isOmsTestLib(true)
  #else
  , m_isOmsTestLib(false)
  #endif 
  , m_cntDumpErrors(0)
  , m_heapThreshold(100)
  , m_omsVersionThreshold(MAX_INT4_SP00)
  , m_tracer(0)
  , m_resetSink() 
  , m_nilOid()
  , m_versionBuffer()  // PTS 1128597
  , m_defaultRawChunkSize(SAPDBMem_RawAllocator::CalcMaxRawChunkSize(OMS_DEFAULT_CHUNK_SIZE))
{
  new(&m_omsAllocatorWrapper) OMS_GlobalAllocatorWrapper(false/*isUserAllocator*/);
  new(&m_userAllocatorWrapper) OMS_GlobalAllocatorWrapper(true/*isUserAllocator*/);
  new(&m_SQLDBCAllocatorWrapper) OMS_GlobalAllocatorWrapperThrowsNothing(false/*isUserAllocator*/);
  new(&m_versionDictionary) OMS_VersionDictionary();
  new(&m_monitorDirectory) OMS_MonitorDirectory(&m_omsAllocatorWrapper);
  new(&m_globalUserAllocatorDirectory) OMS_GlobalUserAllocatorDirectory();
  new(&m_globalAnchorDirectory) OMS_GlobalAnchorDirectory();
  m_nilSeq.gg91SetNilRef();  
}

void OMS_Globals::InitSingletons()
{
  if (m_globalsInstance == NULL) 
    m_globalsInstance = new(&omsGlobalsInstanceSpace) OMS_Globals();
  if (KernelInterfaceInstance == NULL) 
    KernelInterfaceInstance = &OMS_KernelDefaultInterface::Instance();
  if (m_libOmsInterfaceInstance == NULL)
    m_libOmsInterfaceInstance = new(&omsLibOmsInterfaceInstanceSpace) OMS_LibOmsInterfaceInstance();
}

void OMS_Globals::MakeKnlIdentifier (const char* Source, tsp00_KnlIdentifier& Dest) {
  size_t copyLen = strlen(Source);
  if (copyLen > sizeof(Dest)) {
    OMS_Globals::Throw(e_OMS_identifier_too_long, "OMS_Globals::MakeKnlIdentifier", __MY_FILE__, __LINE__);
    //copyLen = sizeof(Dest);
  }
  SAPDB_MemCopyNoCheck(&Dest[0], Source, copyLen);
  memset(&Dest[copyLen], ' ', sizeof(Dest) - copyLen);
}

/*----------------------------------------------------------------------*/

void OMS_Globals::ResetInfo()
{
  // TODO: this is not perfect, but it will do the job...
  m_versionDictionary.Shutdown();
}

/*----------------------------------------------------------------------*/

void OMS_Globals::Dump(OMS_DumpInterface& dumpObj)
{
  this->DumpVersionDirectory(dumpObj);
  OMS_Session* pSession = REINTERPRET_CAST(OMS_Session*, dumpObj.GetOmsSession());
  if (NULL != pSession)
  {
    pSession->Dump(dumpObj);
  }
}

/*----------------------------------------------------------------------*/

bool OMS_Globals::ActionOnError(tsp00_Int4 errorNo, bool &dump, bool &stop, bool &flush) 
{
  dump = stop = flush = false;

  RTESync_LockedScope sync(omsSyncDumpError);
  for (int ix = 0; ix < m_cntDumpErrors; ++ix)
  {
    if (m_actionOnError[ix].errorNo == errorNo)
    {
      stop  = m_actionOnError[ix].stop;
      flush = m_actionOnError[ix].flush;
      dump  = m_actionOnError[ix].dump;

      // Ensure, that 'dump' and 'stop' are only executed once. 
      // If 'flush' is not switched on, then the whole entry can
      // be deleted
      if (!m_actionOnError[ix].flush){
        while (ix < m_cntDumpErrors - 1){
          m_actionOnError[ix] = m_actionOnError[ix+1];
        }
        --m_cntDumpErrors;
      }
      else {
        m_actionOnError[ix].stop = false;
        m_actionOnError[ix].dump = false;
      }

      return (stop || flush || dump);
    }
  }
  return false;
}

/*----------------------------------------------------------------------*/

IliveCacheSink* OMS_Globals::GetCurrentLcSink()
{
  return KernelInterfaceInstance->GetSinkPtr();
}

/*----------------------------------------------------------------------*/

void OMS_Globals::DumpVersionDirectory(OMS_DumpInterface& dumpObj)
{
  dumpObj.SetDumpLabel(LABEL_OMS_VERSION);
   
  OMS_VersionDictionary::Iter iter(dumpObj.Synchronize() ? OMS_LOCK_SHARED : OMS_NO_LOCK);
  while (true){
    OMS_Context *pContext = iter.GetFirstInSlot();
    if (pContext == NULL){
      break;
    }

    while (pContext){
    pContext->Dump(dumpObj);
      pContext = iter.GetNextInSlot();
    }
  }
}

/*----------------------------------------------------------------------*/

bool OMS_Globals::InSimulator() const
{
    return (m_host == HostSimulator);
}

/*----------------------------------------------------------------------*/

bool OMS_Globals::InProcServer() const
{
  if (m_host == HostSimulator) {
    IliveCacheSink *sink = GetCurrentLcSink();
    return sink && (sink->GetSinkType() == 2);
  } else {
    return (m_host == HostProcServer);
  }
}

/*----------------------------------------------------------------------*/

void OMS_Globals::SetDumpError(tsp00_Int4 e, OMS_DiagnoseAction action)
{
  RTESync_LockedScope sync(omsSyncDumpError);
  for (int i=0; i<m_cntDumpErrors; ++i){
    if (m_actionOnError[i].errorNo == e){
      if (action == OMS_DIAGNOSE_DUMP){
        m_actionOnError[i].dump = true;
      }
      else if (action == OMS_DIAGNOSE_STOP){
        m_actionOnError[i].stop = true;
      }
      else if (action == OMS_DIAGNOSE_FLUSH){
        m_actionOnError[i].flush = true;
      }
      return;
    }
  }
  if (m_cntDumpErrors < MAX_DUMP_ERRORS){
    m_actionOnError[m_cntDumpErrors].errorNo = e;
    if (action == OMS_DIAGNOSE_DUMP){
      m_actionOnError[m_cntDumpErrors].dump = true;
    }
    else if (action == OMS_DIAGNOSE_STOP){
      m_actionOnError[m_cntDumpErrors].stop = true;
    }
    else if (action == OMS_DIAGNOSE_FLUSH){
      m_actionOnError[m_cntDumpErrors].flush = true;
    }
    ++m_cntDumpErrors;
  }
}

/*----------------------------------------------------------------------*/

void OMS_Globals::SetHost(bool isKernel, bool isProcServer)
{
  m_host = isKernel ? HostKernel : (isProcServer ? HostProcServer : HostSimulator);
}

/*----------------------------------------------------------------------*/

void OMS_Globals::SetOmsVersionThreshold(tsp00_Uint4 threshold) /* PTS 1110149 */
{
  // 100 has been wrong default, set to wanted default
  m_omsVersionThreshold = (100 == threshold) ? 2097152 : threshold;
}

/*----------------------------------------------------------------------*/

OMS_TraceInterface* OMS_Globals::GetTracer()
{
  return m_tracer;
}

/*----------------------------------------------------------------------*/

void OMS_Globals::SetTracer(OMS_TraceInterface* pTraceObj)
{
  m_tracer = pTraceObj;
}

/*----------------------------------------------------------------------*/

OMS_ExceptionClass OMS_Globals::MapErrorCodeToExceptionClass(short errorNo)
{
  switch (errorNo) {
    case e_new_failed:              return OMS_EXCEPTION_BAD_ALLOC;
    case e_object_dirty:            return OMS_EXCEPTION_OUT_OF_DATE;
    case e_duplicate_hash_key:
    case e_key_still_visible:       return OMS_EXCEPTION_DUPLICATE_KEY;
    case e_hash_key_in_use:         return OMS_EXCEPTION_KEY_IN_USE;
    case e_OMS_lock_collision:
    case e_OMS_request_timeout:     return OMS_EXCEPTION_LOCK_REQUEST_TIMEOUT;
    case e_object_not_found:
    case e_wrong_object_version:
    case e_wrong_object_state:
    case e_too_old_oid:
    case e_invalid_oid: 
    case e_container_dropped:
    //case e_wrong_class_id:
    //case e_no_next_object:
    //case e_hash_not_found:
    //case e_hash_key_not_found:
    //case e_unknown_consist_vers:
    //case e_obj_history_not_found:
    //case e_illegal_object_length:
    //case e_illegal_object_pos:
    //case e_consview_cancelled:
                                    return OMS_EXCEPTION_OBJECT_NOT_FOUND;
    case e_packed_out_of_range:     return OMS_EXCEPTION_OVERFLOW;
    case e_unknown_guid:            return OMS_EXCEPTION_CONTAINER_ERROR;
    case e_OMS_cancelled:           return OMS_EXCEPTION_CANCELLED;
    case e_requested_dbperror:      return OMS_EXCEPTION_PROVOKED_ERROR;
    case e_internal_obj_link_corruption: return OMS_EXCEPTION_INVALID_OBJECT; 
    default :                       return OMS_EXCEPTION_DEFAULT;
  }
}

/*----------------------------------------------------------------------*/

void OMS_Globals::Throw(short errorNo, const char* msg, const char* pFile, unsigned int line, OMS_Session* pSession)
{
  Throw(errorNo, msg, OMS_ObjectId8(), pFile, line, pSession);
}

/*----------------------------------------------------------------------*/

void OMS_Globals::Throw(short errorNo, const char* msg, const OMS_ObjectId8& oid, const char* pFile, unsigned int line, OMS_Session* pSession)
{
  IliveCacheSink* pLcSink = NULL;
  if (pSession == NULL){
    // Get session and sink pointer if not already given
    pLcSink = GetCurrentLcSink();
    tsp00_TaskId     taskId;
    pasbool*         pToCancel;
    bool             optimizedStreamCommunication; // PTS 1130281
    pLcSink->GetDefaultContext(REINTERPRET_CAST(void**, &pSession), &pToCancel, optimizedStreamCommunication, taskId);
  }
  else {
    pLcSink = pSession->m_lcSink;
  }

  // Check whether a certain action for this error-code is registered.
  // Registration is possible via the sql-command:
  //    diagnose object catch <error number> dump|stop|flush
  bool dump, stop, flush;
  if (pLcSink && m_globalsInstance->ActionOnError(errorNo, dump, stop, flush)){
    tsp00_Int4      BufLen      = sizeof(errorNo);
    short           DummyError  = 0;
    if (dump){
      BufLen      = sizeof(errorNo); 
      OMS_HResult hr  = pLcSink->MultiPurpose (m_diagnose, mm_outcopy, &BufLen, 
                                              (unsigned char*)&errorNo, 
                                              &DummyError);
    }
    if (stop){
      BufLen      = sizeof(errorNo);
      OMS_HResult hr  = pLcSink->MultiPurpose (m_diagnose, mm_write_off, &BufLen, 
                                              (unsigned char*)&errorNo, 
                                              &DummyError);
    }
    if (flush){
      BufLen      = sizeof(errorNo);
      OMS_HResult hr  = pLcSink->MultiPurpose (m_diagnose, mm_file, &BufLen, 
                                              (unsigned char*)&errorNo, 
                                              &DummyError);
    }
  }

  // Group errors into error classes
  switch (MapErrorCodeToExceptionClass(errorNo)) {
  case OMS_EXCEPTION_BAD_ALLOC: // PTS 1109338
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw DbpError with ErrorNo=" << errorNo 
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession != 0)
      pSession->IncOutOfMemory();
    throw DbpError(DbpError::DB_ERROR, errorNo, msg, oid, pFile, line);
    break;

  case OMS_EXCEPTION_OUT_OF_DATE:
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsOutOfDate with ErrorNo=" << errorNo
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession)
      pSession->IncOutOfDate();
    throw OmsOutOfDate(oid, msg, pFile, line);
    break;

  case OMS_EXCEPTION_DUPLICATE_KEY:
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsDuplicateKey with ErrorNo=" << errorNo
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw OmsDuplicateKey(errorNo, oid, msg, pFile, line);
    break;

  case OMS_EXCEPTION_KEY_IN_USE:
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsKeyInUse with ErrorNo=" << errorNo 
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw OmsKeyInUse(e_hash_key_in_use, oid, msg, pFile, line);
    break;

  case OMS_EXCEPTION_LOCK_REQUEST_TIMEOUT :
    // NOTE: do not forget to test the same codes in OMS_Context::TryLockResult to prevent
    // omsTryLock from throwing OmsLockTimeout exceptions.
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsLockTimeout with ErrorNo=" << errorNo 
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession)
      pSession->IncTimeout();
    throw OmsLockTimeout(oid, msg, pFile, line);
    break;

  case OMS_EXCEPTION_OBJECT_NOT_FOUND:
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsObjectNotFound with ErrorNo=" << errorNo
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw OmsObjectNotFound(errorNo, oid, msg, pFile, line); 
    break;

  case OMS_EXCEPTION_INVALID_OBJECT:  
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsInvalidObject with ErrorNo=" << errorNo
      << "  Msg=\""  << msg << "\""
      << "  " << oid
      << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw OmsInvalidObject( oid, msg, pFile, line ); 
    break;

  case OMS_EXCEPTION_OVERFLOW:
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsOverflow with ErrorNo=" << errorNo
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw OmsOverflow(errorNo, oid, msg, pFile, line); 
    break;

  case OMS_EXCEPTION_CONTAINER_ERROR:  
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw OmsContainerError with ErrorNo=" << errorNo
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw OmsContainerError(errorNo, oid, msg, pFile, line); 
    break;

  case OMS_EXCEPTION_CANCELLED:  
    {
      bool cancelledByProgram   = false;
      bool provokedByTestkernel = false;
      if (pSession){
        if (pSession->IsCancelledByProgram()){
          cancelledByProgram   = true;
          pSession->SetCancelledByProgram(false);
        }
        if (pSession->IsProvokedByTestkernel()){
          provokedByTestkernel = true;
          pSession->SetProvokedByTestkernel(false);
        }
      }
      if (pLcSink)
        OMS_TRACE(omsTrError, pLcSink, "throw OmsCancelled with ErrorNo=" << errorNo
                                      << "  Msg=\""  << msg << "\""
                                      << "  CancelledByProg=" << (cancelledByProgram?"y":"n")
                                      << "  Provoked=" << (provokedByTestkernel?"y":"n")
                                      << "  (" << pFile << ":" << line << ")");
      if (pSession){
        pSession->IncExceptions();
      }
      throw OmsCancelled(errorNo, msg, pFile, line, cancelledByProgram, provokedByTestkernel);      
    }
    break;

  case OMS_EXCEPTION_PROVOKED_ERROR:  
    {
      bool provokedByTestkernel = false;
      if (pSession && pSession->IsProvokedByTestkernel()){
          provokedByTestkernel = true;
          pSession->SetProvokedByTestkernel(false);
      }
      if (pLcSink)
        OMS_TRACE(omsTrError, pLcSink, "throw OmsProvokedError with ErrorNo=" << errorNo
                                      << "  Msg=\""  << msg << "\""
                                      << "  Provoked=" << (provokedByTestkernel?"y":"n")
                                      << "  (" << pFile << ":" << line << ")");
      if (pSession){
        pSession->IncExceptions();
      }
      throw OmsProvokedError(errorNo, msg, pFile, line, provokedByTestkernel); 
    }
    break;

  default :  // OMS_EXCEPTION_DEFAULT
    if (pLcSink)
      OMS_TRACE(omsTrError, pLcSink, "throw DbpError with ErrorNo=" << errorNo 
                                     << "  Msg=\""  << msg << "\""
                                     << "  " << oid
                                     << "  (" << pFile << ":" << line << ")");
    if (pSession){
      pSession->IncExceptions();
    }
    throw DbpError(DbpError::DB_ERROR, errorNo, msg, oid, pFile, line);
  }
}

/*----------------------------------------------------------------------*/

//PTS 1117690
// strlen on strings of type OmsTypeWyde
int OMS_Globals::WideStrlen(const OmsTypeWyde* s)
{
  int len=-1;
  while (s[++len]);
  return len;
}

/*----------------------------------------------------------------------*/
// PTS 1124170
int OMS_Globals::GetLockIdForVersionDirectory (const OmsVersionId &versionId) const { 
  return m_versionDictionary.GetSingleLockId(versionId);
}

/*----------------------------------------------------------------------*/
// PTS 1124170
bool OMS_Globals::GetLockMethodForVersionDictionary() const{  
  return m_versionDictionary.UseRWLocks();
}

/*----------------------------------------------------------------------*/

void OmsObjectContainer::PrintError(const char* msg, const OmsObjectContainer*p) const
{
  char buf[64];
  IliveCacheSink* pLcSink = OMS_Globals::GetCurrentLcSink();
  DbpBase base(pLcSink);
  base.dbpOpError(msg);
  sp77sprintf(buf, sizeof(buf), "this: %p", this);
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "HashNext: %p", p->m_hashnext);
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "Oid: %d.%d(%d)", p->m_oid.getPno(), p->m_oid.getPagePos(), p->m_oid.getGeneration());
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "ObjSeq: %d.%d", p->m_objseq.gg91RefPno(), p->m_objseq.gg91RefPos());
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "State: %d", p->m_state);
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "VerState: %d", p->m_verstate);
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "BeforeImages: %d", p->GetBeforeImageMask());
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "RevisionNumber: %d", p->GetRevisionNumber());
  base.dbpOpError(buf);
  sp77sprintf(buf, sizeof(buf), "ContainerInfo: %p", p->m_pContainerInfo);
  base.dbpOpError(buf);
}


