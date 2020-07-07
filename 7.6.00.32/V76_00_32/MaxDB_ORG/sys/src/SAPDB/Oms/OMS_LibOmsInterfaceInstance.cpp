/*!
 * \file    OMS_LibOmsInterface.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   OMS LibOmsInterface.
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


#include "Oms/OMS_LibOmsInterfaceInstance.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_SinkCriticalSection.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_VersionDictionary.hpp"
#include "Oms/OMS_MonitorDirectory.hpp"
#include "Oms/OMS_LockEntryHash.hpp"
#include "Oms/OMS_Session.hpp"
#include "liveCache/LVC_LockRequest.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hsp77.h"
#include "hsp78_0.h"
#include "RunTime/RTE_IInterface.hpp"

/*---------------------------------------------------------------------------*/
/*  external Globals                                                         */
/*---------------------------------------------------------------------------*/

externCpp bool omsInUnicodeMode = false;

/*---------------------------------------------------------------------------*/
/*                              Globals                                      */
/*---------------------------------------------------------------------------*/

static bool                    m_mallocMonitor  = false;  // PTS 1110287

RTE_IInterface *RTE_IInterface::it;
RTE_IInterface *RTE_IInterface::Initialize() { return it; }

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::AdviseKernelInterface(LVC_KernelInterface& knlInterface, bool isKernel, bool isUDEServer)
{
    OMS_Globals::KernelInterfaceInstance = &knlInterface;

    RTE_IInterface::it = &knlInterface.GetKernelRunTimeInterface();

    if (!OMS_Globals::m_globalsInstance) 
      OMS_Globals::InitSingletons();
    OMS_Globals::m_globalsInstance->SetHost(isKernel, isUDEServer);    
    omsInUnicodeMode = OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance();

    if (!isUDEServer){
      for (int i=0; i<OMS_Globals::m_globalsInstance->m_versionDictionary.GetVDirSize(); ++i){
        // Get single lock for every hash-array   // PTS 1124533
        void *lockAddress=NULL;
		    LVC_LockRequest lock(LVC_LockRequest::RWLOCK_CREATE, OMS_RWLOCK_AREA, i + OMS_Globals::m_globalsInstance->m_versionDictionary.GetVDirStartCnt(), lockAddress);
        short err = knlInterface.LockRequest(lock);
      }

      // PTS 1124533
      OMS_Globals::m_globalsInstance->m_versionDictionary.m_useRWLocks = (knlInterface.UseReaderWriterLocks() ? true : false);

      OMS_Globals::m_globalsInstance->m_versionBuffer.Advise(&OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper, 10);  
    }
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::InitLibOms(IliveCacheSink* lcSink) 
{
  if (!OMS_Globals::m_globalsInstance) 
    OMS_Globals::InitSingletons();

  OMS_Globals::m_globalsInstance->SetOmsVersionThreshold(OMS_Globals::KernelInterfaceInstance->GetOmsVersionThreshold());
  OMS_LockEntryHash::m_instance.SetTimeout(OMS_Globals::KernelInterfaceInstance->GetLockRequestTimeout());
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::AdviseTracer(OMS_TraceInterface* pTraceObj)
{
  if (!OMS_Globals::m_globalsInstance) 
    OMS_Globals::InitSingletons();
  OMS_Globals::m_globalsInstance->SetTracer(pTraceObj);
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::Dump(OMS_DumpInterface& dumpObj)
{
  OMS_Globals::m_globalsInstance->Dump(dumpObj);
}

/*----------------------------------------------------------------------*/

tsp00_Bool OMS_LibOmsInterfaceInstance::GetFirstVersion(IliveCacheSink*     lcSink,
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
                                                  tsp00_C512&         versionDesc,
                                                  short&              error)   // PTS 1126695
{
  try {
    // Critical section is used to synchronize the access to the global iterator
    // structure. It is NOT used to synchronize access to the version dictionary.
    lcSink->EnterCriticalSection(RGN_VERSION_DIR);

    // Reset the global iterator
    OMS_Globals::m_globalsInstance->m_versionIter.Reset(OMS_LOCK_EXCLUSIVE);
  } 
  catch (DbpError &e){  // PTS 1126700

    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    OMS_Globals::m_globalsInstance->m_versionIter.Stop();
    lcSink->LeaveCriticalSection(RGN_VERSION_DIR);
    error = e.m_errorNo;
    return false;
  }

  return GetNextVersion(lcSink, versionId, createDate, createTime, 
    openDate, openTime, consistentView, isMarked, isOpen, isUnloaded,
    heapUsage, hashSize, versionTree, versionInvTree, versionDesc, error);
}

/*----------------------------------------------------------------------*/

tsp00_Bool OMS_LibOmsInterfaceInstance::GetMonitorInfo(
                                                 IliveCacheSink*       lcSink,
                                                 tsp00_Addr&           handle,
                                                 tsp00_C16&            iid,
                                                 tsp00_Int4&           dispid,
                                                 tgg01_COMMonitorInfo& monInfo)
{
  static OMS_MonitorDirectory::Iterator Iter;
  if (NULL == handle) {
    lcSink->EnterCriticalSection(RGN_MONITOR);
    Iter = OMS_Globals::m_globalsInstance->m_monitorDirectory.begin();
    handle = REINTERPRET_CAST(tsp00_Addr, &Iter);
  }
  if (Iter) {
    Iter.Get(*REINTERPRET_CAST(CLSID*, &iid), dispid, monInfo);
    ++Iter;
    return true;
  }
  else {
    lcSink->LeaveCriticalSection(RGN_MONITOR);
  }
  return false;
}

/*----------------------------------------------------------------------*/

tsp00_Bool OMS_LibOmsInterfaceInstance::GetNextVersion(IliveCacheSink*     lcSink,
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
                                                 tsp00_C512&         versionDesc,
                                                 short&              error)   // PTS 1126695
{
  try {
    const unsigned int MAX_BUFFER_SIZE = 100;
    int                cnt = 0;
    OMS_Context       *ppContext[MAX_BUFFER_SIZE];
    OMS_Context       *pContext = NULL;
    OMS_Session       *pSession = NULL;
    bool               checkForSession = true;

    error = e_ok;
  
    while (true){
      if (OMS_Globals::m_globalsInstance->m_versionBuffer.IsEmpty()){
        // Position the iterator on the next slot in the hash of the version directory.
        pContext = OMS_Globals::m_globalsInstance->m_versionIter.GetFirstInSlot();

        // Read all versions which belong to the next slot in the hash of the version 
        // directory and insert them into the buffer. After all versions have been read 
        // the corresponding lock is freed again.
        while (pContext){
          OMS_Context *pNext = OMS_Globals::m_globalsInstance->m_versionIter.GetNextInSlot();
          if (!pContext->IsDropped()){
            VersionEntry   versionEntry;
            memcpy(&versionEntry.versionId[0], &pContext->GetVersionId()[0], sizeof(OmsVersionId)); 
            versionEntry.createDate     = pContext->GetCreateDate();
            versionEntry.createTime     = pContext->GetCreateTime();
            versionEntry.openDate       = pContext->GetOpenDate();
            versionEntry.openTime       = pContext->GetOpenTime();
            versionEntry.consistentView = pContext->m_consistentView;
            versionEntry.isMarked       = pContext->IsMarked();
            versionEntry.isOpen         = pContext->IsVersionOpen();
            versionEntry.isUnloaded     = pContext->IsUnloaded();
            versionEntry.heapUsage      = pContext->HeapUsed();
            versionEntry.hashSize       = pContext->GetHashSize();
            versionEntry.versionTree    = pContext->VersionFile();
            versionEntry.versionInvTree = pContext->VersionInvFile();
            if (pContext->GetVersionDesc() != NULL){
              versionEntry.hasVersionDesc = true;
              memcpy(&versionEntry.versionDesc[0], pContext->GetVersionDesc(), sizeof(versionEntry.versionDesc));
            }
            else {
              versionEntry.hasVersionDesc = false;
            } 

            OMS_Globals::m_globalsInstance->m_versionBuffer.Push(versionEntry);
          }
          else {
            if (checkForSession && pSession == NULL){
              // Get session if not already determined If no com-routine has been executed
              // before in this connection, then no session exists. In this case, the versions,
              // which are marked as deleted, will not be removed. 
              pasbool         *pToCancel;
              tsp00_TaskId     taskId;
              bool             optimizedStreamCommunictation;
              lcSink->GetDefaultContext(reinterpret_cast<void**>(&pSession), &pToCancel, optimizedStreamCommunictation, taskId);
              checkForSession = false;
            }
 
            // If no session has been found, or if there are container registered in the context for which the corresponding
            // class is not registered in the current session (This happens e.g. if the db-analyzer is running), then the drop 
            // of the version is skipped.
            if (pSession != NULL && pContext->GetContainerDir().CheckForUnregisteredClasses(pSession) == false){
              if (!pContext->IsBoundToTrans()) {
                pContext->MarkBoundToTrans(pSession);
                OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(lcSink, pContext);
                OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(pContext->GetVersionId()); 
                if (cnt < MAX_BUFFER_SIZE){
                  // Remember a pointer to the context for later destruction. As the version is
                  // not registered any more in the version directory, nobody else can change
                  // the pointer.
                  ppContext[cnt++] = pContext;
                }
                else {
                  // To prevent lock contention problems this should never occur. Normally the
                  // destruction of the version should be done outside the exclusive region.
                  // But if it occurs inside, then 'only' performance will degrade.
                  pSession->DropVersionEpilog(pContext);
                }
              }
            }
          }
          pContext = pNext;
        }
        OMS_Globals::m_globalsInstance->m_versionIter.ReleaseSlot();
      }
 
      if (pSession != NULL){
        // Destruct the versions, which are marked as deleted, outside the exclusive region
        for (int i=0; i<cnt; ++i){
          pSession->DropVersionEpilog(ppContext[i]);
        }
        cnt = 0;
      }
    
      if (OMS_Globals::m_globalsInstance->m_versionBuffer.IsEmpty()){
        // All versions have been considered
        lcSink->LeaveCriticalSection(RGN_VERSION_DIR);
        return false;
      }
      else {
        // Return last entry from the buffer
        const VersionEntry *pVersionEntry = OMS_Globals::m_globalsInstance->m_versionBuffer.Pop();
        memcpy(&versionId[0], &pVersionEntry->versionId[0], sizeof(OmsVersionId)); 
        createDate     = pVersionEntry->createDate;
        createTime     = pVersionEntry->createTime;
        openDate       = pVersionEntry->openDate;
        openTime       = pVersionEntry->openTime;
        consistentView = pVersionEntry->consistentView;
        isMarked       = pVersionEntry->isMarked;
        isOpen         = pVersionEntry->isOpen;
        isUnloaded     = pVersionEntry->isUnloaded;
        heapUsage      = pVersionEntry->heapUsage;
        hashSize       = pVersionEntry->hashSize;
        versionTree    = pVersionEntry->versionTree;
        versionInvTree = pVersionEntry->versionInvTree;

        // PTS 1117690
        // Initialize version-desciption
        if (omsInUnicodeMode){
          // initialize with blanks in kernel layout (big endian)
          for (int ix = 0; ix < sizeof(versionDesc); ix += 2)
          {
            versionDesc[ix  ] = 0;
            versionDesc[ix+1] = ' ';
          }
        }
        else {
          memset(&versionDesc[0], ' ', sizeof(versionDesc));
        }
        
        if (pVersionEntry->hasVersionDesc){
          int len = OMS_Globals::WideStrlen(pVersionEntry->versionDesc);
          
          if (omsInUnicodeMode){
            // Source and Target in UCS2 => No Convertion needed but output string could be
            // too long
            int len_byte = (len * sizeof(OmsTypeWyde) > sizeof(versionDesc)
                         ? sizeof(versionDesc)
                         : len * sizeof(OmsTypeWyde));

            SAPDB_MemCopyNoCheck(versionDesc, pVersionEntry->versionDesc, len_byte); 
            
            // check if endianness of maschine is the same as the endianness of the kernel
            if (sp77encodingUCS2Native != sp77encodingUCS2){
              // translate to kernel layout
              for (int ix = 0; ix < len_byte; ix += 2){
                // swap
                char aux = versionDesc[ix];
                versionDesc[ix  ] = versionDesc[ix+1];
                versionDesc[ix+1] = aux;
              }
            }
          }
          else {
            tsp00_Uint4 destBytesWritten;
            tsp00_Uint4 srcBytesParsed;
            tsp78ConversionResult error;

            // Convertion from UCS2 to ASCII
            error = sp78convertString(sp77encodingAscii,              // destEndcoding
                                      versionDesc,                    // destBuffer
                                      sizeof(versionDesc),            // destBufferLengthInBytes
                                      &destBytesWritten,              // destBytesWritten
                                      false,                          // addZeroTerminator
                                      sp77encodingUCS2Native,         // srcEncoding
                                      pVersionEntry->versionDesc,     // srcBuffer
                                      len * sizeof(OmsTypeWyde),      // srcBufferLengthInBytes
                                      &srcBytesParsed);               // srcBytesParsed
          }
        }

        return true;
      }
    }
  } 
  catch (DbpError &e){  // PTS 1126700
    OMS_Globals::m_globalsInstance->m_versionIter.Stop();
    lcSink->LeaveCriticalSection(RGN_VERSION_DIR);
    error = e.m_errorNo;
    return false;
  }
}

/*----------------------------------------------------------------------*/

// PTS 1117690
tsp00_Bool OMS_LibOmsInterfaceInstance::GetNextDiagnoseEntry (
                       tsp00_Int2       &phase,
                       IliveCacheSink   *pLcSink,
                       tsp00_Int4       &session,
                       tsp00_C24        &component,
                       tsp00_C512       &entry,
                       short            &error) 
{
  error = 0;
  session = 0; 
  memset (component, 0, sizeof(component));
  memset (entry,     0, sizeof(entry));
  
  if (phase == 5 || phase == 6){
    // Read global container directory
    bool found = OMS_Globals::m_globalsInstance->m_versionDictionary.NextDiagEntry (pLcSink, phase==5, entry, sizeof(entry), error);
    if (found){
      session = 0; // = Global
      strcpy(component, "Version Directory");
      phase = 6;
      return true;
    }
    else {
      phase = 7;
    }
  }

  return false;
}

/*----------------------------------------------------------------------*/

bool OMS_LibOmsInterfaceInstance::IsMonitorOn() const 
{
  return m_mallocMonitor;
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::InspectTimeout() // PTS 1110287
{
  OMS_LockEntryHash::m_instance.InspectTimeout();
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::ResetMonitor(IliveCacheSink* lcSink)
{
  OMS_SinkCriticalSection cs(lcSink, RGN_MONITOR);
  cs.Enter();
  OMS_Globals::m_globalsInstance->m_monitorDirectory.Reset();
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::SetDumpError(tsp00_Int4 errorNo, OMS_DiagnoseAction action)
{
  OMS_Globals::m_globalsInstance->SetDumpError(errorNo, action);
}

/*----------------------------------------------------------------------*/
// PTS 1110287
bool OMS_LibOmsInterfaceInstance::NextOmsLockObjInfo(void** pVoidLockInfo, tgg01_OmsLockInfo& lockInfo)
{
  return OMS_LockEntryHash::m_instance.NextLockInfo(pVoidLockInfo, lockInfo);
}

/*----------------------------------------------------------------------*/

bool OMS_LibOmsInterfaceInstance::SetTraceLevel(const char* lvl, bool enable)
{
  return TraceLevel_co102.Set(lvl, enable);
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::StartStopMonitor(bool doStart) {
  m_mallocMonitor = doStart;
}

/*----------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::UnAdviseKernelInterface()
{
    OMS_Globals::KernelInterfaceInstance = NULL;
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::UserAllocatorCtl( int msg, void  *param){
    OMS_Session     *pSession = NULL;
    pasbool         *pToCancel;
    tsp00_TaskId     taskId;
    bool             optimizedStreamCommunictation;
    OMS_Globals::m_globalsInstance->GetCurrentLcSink()->GetDefaultContext(reinterpret_cast<void**>(&pSession), &pToCancel, optimizedStreamCommunictation, taskId);
    if(pSession != NULL)
      pSession->UserAllocatorCtl(msg,param);
}

/*----------------------------------------------------------------------*/

bool OMS_LibOmsInterfaceInstance::VersionUnloaded()
{
  return OMS_Globals::m_globalsInstance->m_versionDictionary.UnloadOldestVersion(OMS_Globals::GetCurrentLcSink());;
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictInsert(
  const OmsVersionId &versionId, 
  const OMS_Context  *pVersionContext,
  tgg00_BasisError   &error)
{
  error = OMS_Globals::m_globalsInstance->m_versionDictionary.InsertVersion(versionId, 
    const_cast<OMS_Context*>(pVersionContext));
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictFind(
  const OmsVersionId  &versionId,
  OMS_Context        **ppVersionContext)
{
 *ppVersionContext = OMS_Globals::m_globalsInstance->m_versionDictionary.FindVersion(versionId);
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictDrop(
  const OmsVersionId &versionId, 
  tgg00_BasisError   &error)
{
  error = OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(versionId);
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictShutdown()
{
  OMS_Globals::m_globalsInstance->m_versionDictionary.Shutdown();
}

/*-------------------------------------------------------------------------*/

bool OMS_LibOmsInterfaceInstance::VersionDictUnloadOldestVersion()
{
  IliveCacheSink *pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();
  return OMS_Globals::m_globalsInstance->m_versionDictionary.UnloadOldestVersion(pSink);
}


/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictMarkNotUnloadable(
  OMS_Context    *pVersionContext)
{
  IliveCacheSink *pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();
  OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(pSink, pVersionContext);
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictMarkUnloadable(
  OMS_Context  *pVersionContext)
{
  OMS_Globals::m_globalsInstance->m_versionDictionary.MarkUnloadable(pVersionContext);
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictCreateIter(
  OMS_LockMode   lockMode, 
  void         **ppIter, 
  OMS_Context  **ppContext)
{
  *ppIter = OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(sizeof(OMS_VersionDictionary::Iter));
  OMS_VersionDictionary::Iter* pIter = reinterpret_cast<OMS_VersionDictionary::Iter*>(*ppIter);
  
  pIter->Reset(lockMode);
  *ppContext = pIter->GetFirstInSlot();

  if (*ppContext == NULL){
    // Iterator is empty
    OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(*ppIter);
    *ppIter    = NULL;
    *ppContext = NULL;
  }
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictGetNext(
  void         *p, 
  OMS_Context **ppContext)
{
  OMS_VersionDictionary::Iter* pIter = reinterpret_cast<OMS_VersionDictionary::Iter*>(p);

  // Is there another version in the same slot of the hash?
  *ppContext = pIter->GetNextInSlot(); 
  if (*ppContext == NULL){
    // Is there another version in another slot?
    *ppContext = pIter->GetFirstInSlot();
    if (*ppContext == NULL) {
      // Iterator is empty
      OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(p);
      *ppContext = NULL;
    }
  }
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::VersionDictDestructIter(
  void  *p) 
{
  if (p == NULL)
    return;

  OMS_VersionDictionary::Iter* pIter = reinterpret_cast<OMS_VersionDictionary::Iter*>(p);
  
  // Ensure that all locks are freed
  pIter->Stop();
    
  // Iterator is empty
  OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(pIter);
}

/*-------------------------------------------------------------------------*/

void OMS_LibOmsInterfaceInstance::CancelVersion(
  tgg91_TransNo &ConsistViewNo) 
{
  OMS_VersionDictionary::Iter VersionIter(OMS_LOCK_EXCLUSIVE); 
  bool found = false;
  while (!found)
  {
    OMS_Context* pContext = VersionIter.GetFirstInSlot();
    if (NULL == pContext)
      break;

    while (pContext && !found)
    {
      OMS_Context *pNext = VersionIter.GetNextInSlot();

      if (pContext->m_consistentView == ConsistViewNo)
      {
        found = true;

        IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

        OMS_Session     *pSession;
        pasbool         *pToCancel;
        tsp00_TaskId     taskId;
        bool             optimizedStreamCommunictation;
        pSink->GetDefaultContext(reinterpret_cast<void**>(&pSession), &pToCancel, optimizedStreamCommunictation, taskId);

        DbpBase b(pSink);
        char versionId[sizeof(OmsVersionId)+1];
        SAPDB_MemCopyNoCheck(&versionId[0], &pContext->GetVersionId()[0], sizeof(OmsVersionId));
        versionId[sizeof(OmsVersionId)] = 0;

        if (!pContext->IsBoundToTrans() && pSession != NULL)
        {
          pContext->MarkBoundToTrans(pSession);
          OMS_Globals::m_globalsInstance->m_versionDictionary.MarkNotUnloadable(pSink, pContext);
          OMS_Globals::m_globalsInstance->m_versionDictionary.DropVersion(pContext->GetVersionId());

          tgg01_OmsVersionContext  m_versionContext;
          m_versionContext.ovc_tree          = pContext->VersionFile();
          m_versionContext.ovc_index_tree    = pContext->VersionInvFile();
          m_versionContext.ovc_key           = pContext->CurrentVersionKey();
          m_versionContext.ovc_trans_version = ConsistViewNo;
          tgg00_BasisError DBError;
          OMS_HResult hr = pSink->DropVersion((unsigned char*) &m_versionContext, &DBError);
          if (0 != DBError)
          {
            // just write a message into knldiag and ignore error
            b.dbpOpError("Cancel OMS version %s: error %d ignored", DBError);
          }
          else {
            b.dbpOpMsg("Cancel OMS version %s: Dropped", &versionId[0]);
          }
          pContext->DeleteSelf();
        }
        else
        {
          if (pContext->IsBoundToTrans())
            b.dbpOpMsg("Cancel OMS version %s (Context still bound): Only marked as 'Dropped'", &versionId[0]);
          else if (pSession == NULL)
            b.dbpOpMsg("Cancel OMS version %s (Session equals NULL): Only marked as 'Dropped'", &versionId[0]);
          pContext->MarkDropped();
        }
      }

      pContext = pNext;
    }
  }
}

/*-------------------------------------------------------------------------*/
