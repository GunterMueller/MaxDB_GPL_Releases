/*!
 * \file    OMS_VersionDictionary.cpp
 * \author  MarkusSi, Roterring
 * \brief   Implementation of the dictionary for versions
 */

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_VersionDictionary.hpp"
#include "Oms/OMS_SinkCriticalSection.hpp"
#include "Oms/OMS_Context.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "liveCache/LVC_LockRequest.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hsp77.h"

/*--------------------------------------------------------------------------*/
/*       Implementation of  OMS_VersionDictionary                           */
/*--------------------------------------------------------------------------*/

OMS_VersionDictionary::OMS_VersionDictionary() 
  : m_UnloadableVersions(),
    m_useRWLocks(true),
    m_pIter(NULL),
    m_iter()
{
  for (int i=0; i<VDIR_SIZE; ++i){
    // Initialize buckets of hash-array
    m_pVersionDictionary[i] = NULL;
  }
}

/*---------------------------------------------------------------------------*/

tgg00_BasisError OMS_VersionDictionary::DropVersion  (const OmsVersionId&  versionId)
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    int slot = HashValue(versionId);
    OMS_Context *pCurr = m_pVersionDictionary[slot];
    OMS_Context *pPred = NULL;
    while (pCurr){
      if (!memcmp(pCurr->GetVersionId(), versionId, sizeof(OmsVersionId))){
        if (pPred != NULL)
          pPred->SetNext(pCurr->GetNext());
        else
          m_pVersionDictionary[slot] = pCurr->GetNext();

        // PTS 1125433
        pCurr->SetNext(NULL);
        
        OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Remove version from directory: " <<
          OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
        
        return e_ok;
      }
      pPred = pCurr;
      pCurr = pCurr->GetNext();
    }
    return e_unknown_version;
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    tgg00_BasisError  error;  
    pSink->VersionDictDrop(versionId, error);

    return error;
  }
}

/*---------------------------------------------------------------------------*/

OMS_Context* OMS_VersionDictionary::FindVersion (const OmsVersionId& versionId) 
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    OMS_Context *pCurr = m_pVersionDictionary[HashValue(versionId)];
    while (pCurr){
      if (!memcmp(pCurr->GetVersionId(), versionId, sizeof(OmsVersionId))){
        return pCurr;
      }
      pCurr = pCurr->GetNext();
    }
    return NULL;
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    OMS_Context *pVersionContext;
    pSink->VersionDictFind(versionId, &pVersionContext);

    return pVersionContext;
  }
}

/*---------------------------------------------------------------------------*/

tgg00_BasisError OMS_VersionDictionary::InsertVersion(const OmsVersionId& versionId,OMS_Context* pContext) 
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    int slot = HashValue(versionId);
    if (m_pVersionDictionary[slot] == NULL){
      OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Insert version into directory (I): " <<
                 OMS_CharBuffer(versionId, sizeof(OmsVersionId)));

      m_pVersionDictionary[slot] = pContext;
    }
    else {
      OMS_Context* pCurr = m_pVersionDictionary[slot];
      while (true) {
        // Check if there is already a version with the same name
        if (!memcmp(pCurr->GetVersionId(), versionId, sizeof(OmsVersionId))){
          OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Insert into directory failed because of duplicate name: " <<
                     OMS_CharBuffer(versionId, sizeof(OmsVersionId)));
          
          return e_OMS_duplicate_name;
        }

        if (pCurr->GetNext() == NULL){
          // Append new entry at end of list
          pCurr->SetNext(pContext);

          OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Insert version into directory (II): " <<
                     OMS_CharBuffer(versionId, sizeof(OmsVersionId)));

          return e_ok;
        }
        else {
          pCurr = pCurr->GetNext();
        }
      }
    }

    return e_ok;
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    tgg00_BasisError  error;  
    pSink->VersionDictInsert(versionId, pContext, error);

    return error;
  }
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::Shutdown() 
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Version directory is shut down.");

    for (int i=0; i<VDIR_SIZE; ++i){
      m_pVersionDictionary[i] = NULL;
    }
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    pSink->VersionDictShutdown();
  }
}

/*---------------------------------------------------------------------------*/

bool OMS_VersionDictionary::UnloadOldestVersion(IliveCacheSink* lcSink) 
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){

    //reassign pointer to session, as it may not be valid, if called from bad alloc handling
    pasbool*     pToCancel;
    tsp00_TaskId taskId;
    bool         optimizedStreamCommunication; // PTS 1130281
    OMS_Session *pSession;
    OMS_Globals::GetCurrentLcSink()->GetDefaultContext(REINTERPRET_CAST(void**, &pSession), &pToCancel, optimizedStreamCommunication, taskId);
    if (pSession == NULL){
      // Session pointer not set, can not unload any version 
      OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Session pointer not set, cannot unload version.");
      return false;
    }

    DbpBase b(lcSink);
    bool insertedIntoTransVersion = false;
    char versionId[sizeof(OmsVersionId)+1];
    OMS_Context *pOldContext = pSession->CurrentContext();
    OMS_Context *pContext    = NULL;

    { // Begin of exclusive critical section.   // PTS 1124170, 1133629
      // Acquire all locks on the version dictionary.
      int         pLockId[VDIR_SIZE];
      int         size;
      GetAllLockIds(&pLockId[0], size);
      OMS_InternalMultiScope scope(pSession, OMS_RWLOCK_AREA, &pLockId[0], size, true/*exclusive*/); 

      OMS_SinkCriticalSection unloadRegion(lcSink, RGN_UNLOAD_VERSION);
      unloadRegion.Enter();
      pContext = m_UnloadableVersions.RemoveVersion(NULL);

      // If a version is found, then bound version to current session, to prevent access by 
      // any other session. After having been bound, the locks on the version directory can
      // be released. Unloading itself is done outside the critical section.
      if (pContext){
        SAPDB_MemCopyNoCheck(&versionId[0], &pContext->GetVersionId()[0], sizeof(OmsVersionId));
        versionId[sizeof(OmsVersionId)] = 0;

        if (pContext->IsBoundToTrans() && !pSession->VersionBoundByMe(pContext)){
          b.dbpOpError("Unloading oms version %s skipped, as version is bound to another session", &versionId[0]);
          return true;
        }
        else {
          pSession->SetContext(pContext);
          
          if (!pContext->IsBoundToTrans()){ 
            pSession->InsertTransVersion(pContext, /*create=*/false);
            insertedIntoTransVersion = true;
          }
        }
      }
    }  // End of exclusive critical section.


    if (NULL != pContext){
      b.dbpOpMsg("Unloading oms version %s because of bad allocation", &versionId[0]);
      OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "Unloading version: " <<
        OMS_CharBuffer(pContext->GetVersionId(), sizeof(OmsVersionId)));

      pContext->UnLoad(/*badAllocHandling=*/true);

      if (insertedIntoTransVersion){
        pSession->RemoveFromTransVersion(pContext);
      }
      pSession->SetContext(pOldContext);

      pContext->SetSession(NULL);
      pContext->GetContainerDir().ForbitChangeOfClassEntryPtr();
      pContext->m_boundToTrans = false;

      return true;
    }
    else {
      OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "No version found for unloading.");
      return false;
    }
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    bool unloaded;
    pSink->VersionDictUnloadOldestVersion(unloaded);
    return unloaded;
  }
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::MarkNotUnloadable(IliveCacheSink* lcSink, OMS_Context* pContext)
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    OMS_SinkCriticalSection region(lcSink, RGN_UNLOAD_VERSION);
    region.Enter();
    OMS_Context *p = m_UnloadableVersions.RemoveVersion(pContext);

    if (p){
      OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Removed from list of unloadable versions: " <<
                 OMS_CharBuffer(pContext->GetVersionId(), sizeof(OmsVersionId)));
    }
    else {
      OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Version was not found in list of unloadable versions: " <<
                 OMS_CharBuffer(pContext->GetVersionId(), sizeof(OmsVersionId)));
    }
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    pSink->VersionDictMarkNotUnloadable(pContext);
  }
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::MarkUnloadable(OMS_Context* pContext)
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    OMS_SinkCriticalSection region(pContext->GetSession()->m_lcSink, RGN_UNLOAD_VERSION);
    region.Enter();
    m_UnloadableVersions.InsertVersion(pContext);

    OMS_TRACE (omsTrVersion, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), " - Version was inserted into list of unloadable versions: " <<
               OMS_CharBuffer(pContext->GetVersionId(), sizeof(OmsVersionId)));
  }
  else {
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();

    pSink->VersionDictMarkUnloadable(pContext);
  }
}

/*---------------------------------------------------------------------------*/

int OMS_VersionDictionary::GetSingleLockId(const OmsVersionId &versionId) const{
  return HashValue(versionId) + GetVDirStartCnt();
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::GetMultiLockIds(const OmsVersionId** ppVersionId, int &size, int *pLockId) const
{
  int i, j;
  int tmp[VDIR_SIZE];

  // Initialize temporary array
  for (i=0; i<VDIR_SIZE; ++i){
    tmp[i] = -1;
  }

  // Determine hash slots which corresponds to the given version-ids. 
  for (i=0; i<size; ++i){
    int slot = HashValue(*(ppVersionId[i]));
    if (tmp[slot] == -1)
      tmp[slot] = slot + GetVDirStartCnt();
  }

  // "Compress" the array
  for (i=0,j=0; i<VDIR_SIZE; ++i){
    if (tmp[i] != -1)
      pLockId[j++] = tmp[i];
  }

  // Return the number of lock-ids
  size = j;
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::GetAllLockIds(int *pLockId, int &size) const{
  for (int i=0; i<VDIR_SIZE; ++i)
    pLockId[i] = i + GetVDirStartCnt();
  size   = VDIR_SIZE;
}

/*---------------------------------------------------------------------------*/

bool OMS_VersionDictionary::isLockedByOwn(int slot) const {  
  int lockId = slot + GetVDirStartCnt();
  if (m_useRWLocks){
    void *lockAddress=NULL;
    LVC_LockRequest lock(LVC_LockRequest::RWLOCK_IS_LOCKED, OMS_RWLOCK_AREA, lockId, lockAddress);
    short err = co10_GetKernelInterface()->LockRequest(lock);
    return (err == 0 ? true : false);
  }
  else {
    bool inRgn = false;
    short error;
    OMS_Globals::m_globalsInstance->GetCurrentLcSink()->IsInRegion(lockId, inRgn, error);
    if (error != e_ok){
      OMS_Globals::Throw(e_OMS_not_implemented, "OMS_VersionDictionary::isLockedByOwn", __FILE__, __LINE__);
    }
    return inRgn;
  }
}

/*---------------------------------------------------------------------------*/

bool OMS_VersionDictionary::NextDiagEntry (IliveCacheSink* pLcSink, bool first, char* pDiagEntry, int size, short &error)
{
  try {
    OMS_Context *pContext = NULL;

    if (first){
      // Critical section is used to synchronize the access to the global iterator
      // structure. It is NOT used to synchronize access to the version dictionary.
      pLcSink->EnterCriticalSection(RGN_VERSION_DIR);

      // Reset iterator to the first element
      m_iter.Reset(OMS_LOCK_EXCLUSIVE);
      pContext = m_iter.GetFirstInSlot();
    } 
    else {
      // Increment iterator in the same slot
      pContext = m_iter.GetNextInSlot();
      if (!pContext){
        // If no next entry is found in the same slot, then check whether
        // a next entry might be found in another slot
        pContext = m_iter.GetFirstInSlot();
      }
      if (!pContext){
        // iterator has reached its end
        pLcSink->LeaveCriticalSection(RGN_VERSION_DIR);
        return false;
      }
    }

    char cview[13], cdate[9], odate[9], ctime[9], otime[9];
    unsigned char*p = (unsigned char*)&pContext->m_consistentView;
    for (int i=0, j=0; i<6; ++i){
      char hi = (p[i] >> 4);
      cview[j++] = (hi < 10 ? '0'+hi : 'A'+hi-10);
      char lo = (p[i] & 0x0f);
      cview[j++] = (lo < 10 ? '0'+lo : 'A'+lo-10);
    }
    cview[12] = 0;
    SAPDB_MemCopyNoCheck(&cdate[0], &pContext->GetCreateDate(), 8);  cdate[8] = 0;
    SAPDB_MemCopyNoCheck(&odate[0], &pContext->GetOpenDate(), 8);    odate[8] = 0;
    SAPDB_MemCopyNoCheck(&ctime[0], &pContext->GetCreateTime(), 8);  ctime[8] = 0;
    SAPDB_MemCopyNoCheck(&otime[0], &pContext->GetOpenTime(), 8);    otime[8] = 0;
    sp77sprintf(pDiagEntry, size, "ID:%.24s  Created:%.8s %.8s  Opened:%.8s %.8s  CView:0X%.12s  Open:%s  BoundToTaskId:%d  Unloaded:%s  Dropped:%s  Next:%#X  NextUnloaded:%#X",
      pContext->GetVersionId(), 
      cdate, ctime, odate, otime,
      cview,
      pContext->IsVersionOpen()?"yes":"no",
      pContext->IsBoundToTrans()?pContext->GetSession()->GetTaskId():0,
      pContext->IsUnloaded()?"yes":"no",
      pContext->IsDropped()?"yes":"no",
      pContext->m_next, pContext->m_nextUnloaded);
    return true;
  }
  catch (DbpError &e){  
    DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
    if (pCBInterface){
      pCBInterface->dbpCaughtError(e);
    }

    OMS_Globals::m_globalsInstance->m_versionIter.Stop();
    pLcSink->LeaveCriticalSection(RGN_VERSION_DIR);
    error = e.m_errorNo;
    return false;
  }
}





/*--------------------------------------------------------------------------*/
/*       Implementation of  CVersionDirctionary_co17::Iter                  */
/*--------------------------------------------------------------------------*/


OMS_VersionDictionary::Iter::Iter()
{
  m_slot     = -1;
  m_lockMode = OMS_NO_LOCK;
  m_pCurr    = NULL;
  m_pIter    = NULL;
  m_locked   = false;

  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "OMS_VersionDictionary::Iter::CTOR");
}
  
OMS_VersionDictionary::Iter::Iter(OMS_LockMode lockMode)
{
  m_slot     = -1;
  m_lockMode = lockMode;
  m_pCurr    = NULL;
  m_pIter    = NULL;
  m_locked   = false;

  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "OMS_VersionDictionary::Iter::CTOR");
}
  
/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::Iter::Reset(OMS_LockMode lockMode)
{
  m_lockMode   = lockMode;
  m_slot       = -1;
  m_pCurr      = NULL;
  m_pIter      = NULL;
  m_locked     = false;

  if (OMS_Globals::m_globalsInstance->InProcServer()){
    if (m_pIter != NULL){
      OMS_Globals::Throw(e_invalid_iterator, "OMS_VersionDictionary::Iter::first", __MY_FILE__, __LINE__);
    }
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();
    pSink->VersionDictCreateIter(lockMode, &m_pIter, &m_pCurr);
  }
}

/*---------------------------------------------------------------------------*/
OMS_VersionDictionary::Iter::~Iter()
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "BEGIN OMS_VersionDictionary::Iter::DTOR-1-");

    if (m_locked){
      // Release lock on the current slot
      LeaveRgn();
    }

    //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "END OMS_VersionDictionary::Iter::DTOR-2-");
  }
  else {
    if (m_pIter != NULL){
      IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();
      pSink->VersionDictDestructIter(m_pIter);
    }
  }
}

/*---------------------------------------------------------------------------*/

OMS_Context* OMS_VersionDictionary::Iter::GetFirstInSlot()
{
  if (m_pCurr != NULL){
    throw DbpError(DbpError::DB_ERROR, e_invalid_iterator, "OMS_VersionDictionary::Iter::GetFirstInSlot", __MY_FILE__, __LINE__);
  }

  // There might be still a lock on the previous slot; release it now
  if (m_locked){
    LeaveRgn();
  }

  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "BEGIN OMS_VersionDictionary::Iter::GetFirstInSlot");
   
    OMS_VersionDictionary& VD = OMS_Globals::m_globalsInstance->m_versionDictionary;

    ++m_slot;
    while (m_slot < VDIR_SIZE){
      // Get appropriate lock on the current slot
      EnterRgn();

      if (VD.m_pVersionDictionary[m_slot] != NULL){
        m_pCurr = VD.m_pVersionDictionary[m_slot];
        break;
      }

      // Release lock on the current slot
      LeaveRgn();

      ++m_slot;
    }

    //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "END OMS_VersionDictionary::Iter::GetFirstInSlot" << (m_pCurr==0 ? " = EMPTY" : ""));
  }
  else {
    if (m_pIter != NULL){
      OMS_Globals::Throw(e_invalid_iterator, "OMS_VersionDictionary::Iter::Reset", __MY_FILE__, __LINE__);
    }
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();
    pSink->VersionDictCreateIter(m_lockMode, &m_pIter, &m_pCurr);
  }

  return m_pCurr;
}

/*---------------------------------------------------------------------------*/

OMS_Context* OMS_VersionDictionary::Iter::GetNextInSlot()
{
  if (!OMS_Globals::m_globalsInstance->InProcServer()){
    //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "BEGIN OMS_VersionDictionary::Iter::GetNextInSlot" << (dropCurr ? " DropCurr" : ""));
  
    OMS_VersionDictionary& VD = OMS_Globals::m_globalsInstance->m_versionDictionary;

    if (m_pCurr == NULL){
      OMS_Globals::Throw(e_invalid_iterator, "OMS_VersionDictionary::Iter::GetNextInSlot", __MY_FILE__, __LINE__);
    }

    m_pCurr = m_pCurr->GetNext();

    //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "END OMS_VersionDictionary::Iter::getNext" << (m_pCurr==0 ? " = EMPTY" : ""));
  }
  else {
    if (m_pIter == NULL){
      OMS_Globals::Throw(e_invalid_iterator, "OMS_VersionDictionary::Iter::getNext-2-", __MY_FILE__, __LINE__);
    }
    IliveCacheSink* pSink = OMS_Globals::m_globalsInstance->GetCurrentLcSink();
    pSink->VersionDictGetNext(m_pIter, &m_pCurr);
  }

  return m_pCurr;
}

/*---------------------------------------------------------------------------*/
    
void OMS_VersionDictionary::Iter::ReleaseSlot()
{
  m_pCurr = NULL;

  if (m_locked){
    LeaveRgn();
  }
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::Iter::EnterRgn()
{
  OMS_VersionDictionary& VD = OMS_Globals::m_globalsInstance->m_versionDictionary;

  if (m_pCurr != NULL || m_slot >= VDIR_SIZE){
    OMS_Globals::Throw(e_invalid_iterator, "OMS_VersionDictionary::Iter::enterRgn", __MY_FILE__, __LINE__);
    return;
  }

  // ONLY for debugging
  //if (VD.isLockedByOwn(m_slot)){
  //  DebugBreak();  
  //}

  if (m_lockMode == OMS_NO_LOCK)
    return;

  int lockId = m_slot + VD.GetVDirStartCnt();

  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), 
  //  (m_lockMode == OMS_LOCK_EXCLUSIVE ? "Exclusive " : "Shared ") << "Region requested: " << lockId);

  if (VD.m_useRWLocks){
    void *lockAddress=NULL;
    LVC_LockRequest lock(m_lockMode == OMS_LOCK_SHARED ? LVC_LockRequest::RWLOCK_LOCK_SHARED : LVC_LockRequest::RWLOCK_LOCK_EXCLUSIVE, 
                         OMS_RWLOCK_AREA, lockId, lockAddress);
    short err = co10_GetKernelInterface()->LockRequest(lock);
    if (err != e_ok)
      OMS_Globals::Throw(err, "OMS_VersionDictionary::Iter::enterRgn", __MY_FILE__, __LINE__);
  }
  else {
    OMS_Globals::m_globalsInstance->GetCurrentLcSink()->EnterCriticalSection(lockId);
  }

  m_locked = true;

  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), 
  //  (m_lockMode == OMS_LOCK_EXCLUSIVE ? "Exclusive " : "Shared ") << "Region aquired: " << lockId);
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::Iter::LeaveRgn()
{
  OMS_VersionDictionary& VD = OMS_Globals::m_globalsInstance->m_versionDictionary;

  if (m_slot >= VDIR_SIZE){
    OMS_Globals::Throw(e_invalid_iterator, "OMS_VersionDictionary::Iter::leaveRgn", __MY_FILE__, __LINE__);
    return;
  }

  // ONLY for debugging
  //if (!VD.isLockedByOwn(m_slot)){
  //  DebugBreak();  
  //}

  if (m_lockMode == OMS_NO_LOCK)
    return;

  int lockId = m_slot + VD.GetVDirStartCnt();

  if (VD.m_useRWLocks){
    void *lockAddress=NULL;
    LVC_LockRequest lock(m_lockMode == OMS_LOCK_SHARED ? LVC_LockRequest::RWLOCK_UNLOCK_SHARED : LVC_LockRequest::RWLOCK_UNLOCK_EXCLUSIVE, 
                         OMS_RWLOCK_AREA, lockId, lockAddress);
    short err = co10_GetKernelInterface()->LockRequest(lock);
    if (err != e_ok)
      OMS_Globals::Throw(err, "OMS_VersionDictionary::Iter::leaveRgn", __MY_FILE__, __LINE__);
  }
  else {
    OMS_Globals::m_globalsInstance->GetCurrentLcSink()->LeaveCriticalSection(lockId);
  }

  m_locked = false;

  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), 
  //  (m_lockMode == OMS_LOCK_EXCLUSIVE ? "Exclusive " : "Shared ") << "Region freed: " << lockId);
}

/*---------------------------------------------------------------------------*/

void OMS_VersionDictionary::Iter::Stop(){
  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "BEGIN OMS_VersionDictionary::Iter::Stop");

  if (m_locked){
    LeaveRgn();
  }
  
  m_pCurr    = NULL;
  m_slot     = -1;

  //OMS_TRACE(omsTrInterface, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "END OMS_VersionDictionary::Iter::Stop");
}

/*---------------------------------------------------------------------------*/


